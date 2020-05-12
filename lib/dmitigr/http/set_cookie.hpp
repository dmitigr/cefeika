// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_SET_COOKIE_HPP
#define DMITIGR_HTTP_SET_COOKIE_HPP

#include "dmitigr/http/header.hpp"
#include "dmitigr/http/syntax.hpp"
#include <dmitigr/base/debug.hpp>
#include <dmitigr/net/net.hpp>
#include <dmitigr/str/str.hpp>

#include <dmitigr/dt/timestamp.hpp>

#include <locale>
#include <optional>
#include <stdexcept>
#include <string>

namespace dmitigr::http {

/**
 * @ingroup headers
 *
 * @brief Defines an abstraction of the HTTP Set-Cookie header.
 */
class Set_cookie final : public Header {
public:
  /**
   * @brief Constructs the object by parsing the `input`.
   *
   * Examples of valid input are:
   *
   * name=value
   * name=value; Expires=Sat, 06 Jul 2019 13:23:00 GMT
   * name=value; Max-Age=60
   * name=value; Domain=example.com
   * name=value; Path=/docs/web
   * name=value; Secure
   * name=value; HttpOnly
   * name=value; SameSite=Strict
   * name=value; SameSite=Lax
   * name=value; Domain=example.com; Secure; HttpOnly
   *
   * @param input - set-cookie-string.
   *
   * @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Set-Cookie
   */
  explicit Set_cookie(const std::string_view input)
  {
    // According to: https://tools.ietf.org/html/rfc6265#section-4.1

    enum { name, before_value, value_quoted, value_unquoted, after_value_quoted, semicolon, attr_name, attr_value } state = name;

    static const auto is_valid_attr_name_character = [](const char c)
    {
      static const std::locale l{"C"};
      return std::isalpha(c, l) || (c == '-');
    };

    static const auto is_valid_attr_value_character = [](const char c)
    {
      return !detail::is_ctl(c) && c != ';';
    };

    const auto store_name = [&](std::string& str)
    {
      name_ = std::move(str);
      str = {};
    };

    const auto store_value = [&](std::string& str)
    {
      value_ = std::move(str);
      str = {};
    };

    std::string attr_type;
    const auto process_boolean_attr_type = [&](std::string& str)
    {
      str::lowercase(str);
      if (str == "secure")
        is_secure_ = true;
      else if (str == "httponly")
        is_http_only_ = true;
      else
        throw std::runtime_error{"dmitigr::http: invalid attribute name"};
      str = {};
    };
    const auto set_attr_type = [&](std::string& str)
    {
      str::lowercase(str);
      attr_type = std::move(str);
      str = {};
    };

    const auto store_attr_value = [&](std::string& str)
    {
      if (str.empty())
        throw std::runtime_error{"dmitigr::http: empty values of attribute are not allowed"};

      DMITIGR_ASSERT(attr_type != "secure" && attr_type != "httponly");
      if (attr_type == "expires") {
        expires_ = dt::Timestamp::from_rfc7231(str);
      } else if (attr_type == "max-age") {
        std::size_t pos{};
        max_age_ = std::stoi(str, &pos);
        if (pos != str.size())
          throw std::runtime_error{"dmitigr::http: invalid value of the Max-Age attribute"};
      } else if (attr_type == "domain") {
        const auto pos = str.find_first_not_of('.'); // ignore leading dots
        if (pos != std::string::npos) {
          const std::string host{str.data() + pos, str.size() - pos};
          if (!net::is_hostname_valid(host))
            throw std::runtime_error{"dmitigr::http: invalid value of the Domain attribute"};
        }
        domain_ = std::move(str);
      } else if (attr_type == "path") {
        if (str.front() != '/')
          throw std::runtime_error{"dmitigr::http: invalid value of the Path attribute"};
        path_ = std::move(str);
      } else if (attr_type == "samesite") {
        str::lowercase(str);
        if (str == "strict")
          same_site_ = Same_site::strict;
        else if (str == "lax")
          same_site_ = Same_site::lax;
        else
          throw std::runtime_error{"dmitigr::http: invalid value of the SameSite attribute"};
      } else
        throw std::runtime_error{"dmitigr::http: unknown attribute name"};

      str = {};
    };

    std::string extracted;
    for (const auto c : input) {
      switch (state) {
      case name:
        if (c == '=') {
          store_name(extracted);
          state = before_value;
          continue; // skip =
        } else if (!detail::rfc6265::is_valid_token_character(c))
          throw std::runtime_error{"dmitigr::http: invalid cookie name"};
        break;

      case before_value:
        if (c == ';') {
          // Empty value is okay.
          state = semicolon;
          continue; // skip ;
        } else if (c == '"') {
          state = value_quoted;
          continue; // skip "
        } else if (detail::rfc6265::is_valid_token_character(c)) {
          state = value_unquoted;
        } else
          throw std::runtime_error{"dmitigr::http: invalid cookie value"};
        break;

      case value_quoted:
        if (c == '"') {
          state = after_value_quoted;
          continue; // skip "
        } else if (!detail::rfc6265::is_valid_cookie_octet(c))
          throw std::runtime_error{"dmitigr::http: invalid cookie value"};
        break;

      case value_unquoted:
        if (c == ';') {
          store_value(extracted);
          state = semicolon;
          continue; // skip ;
        } else if (!detail::rfc6265::is_valid_cookie_octet(c))
          throw std::runtime_error{"dmitigr::http: invalid cookie value"};
        break;

      case after_value_quoted:
        if (c == ';') {
          store_value(extracted);
          state = semicolon;
          continue; // skip ;
        } else
          throw std::runtime_error{"dmitigr::http: no semicolon after the quoted value"};
        break;

      case semicolon:
        if (c == ' ') {
          state = attr_name;
          continue; // skip space
        } else
          throw std::runtime_error{"dmitigr::http: no space after the semicolon"};
        break;

      case attr_name:
        if (c == ';') {
          process_boolean_attr_type(extracted);
          state = semicolon;
          continue; // skip ;
        } else if (c == '=') {
          set_attr_type(extracted);
          state = attr_value;
          continue; // skip =
        } else if (!is_valid_attr_name_character(c))
          throw std::runtime_error{"dmitigr::http: invalid attribute name"};
        [[fallthrough]];

      case attr_value:
        if (c == ';') {
          store_attr_value(extracted);
          state = semicolon;
          continue; // skip ;
        } else if (!is_valid_attr_value_character(c))
          throw std::runtime_error{"dmitigr::http: invalid attribute value"};
      }

      extracted += c;
    }

    switch (state) {
    case value_unquoted:
      [[fallthrough]];
    case after_value_quoted:
      store_value(extracted);
      break;
    case attr_name:
      process_boolean_attr_type(extracted);
      break;
    case attr_value:
      store_attr_value(extracted);
      break;
    default:
      throw std::runtime_error{"dmitigr::http: invalid input (set-cookie-string)"};
    }

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @overload
   *
   * @par Requires
   * `(is_valid_cookie_name(name) && is_valid_cookie_value(value))`.
   */
  Set_cookie(std::string name, std::string value)
    : name_{std::move(name)}
    , value_{std::move(value)}
  {
    DMITIGR_REQUIRE(is_valid_cookie_name(name_) && is_valid_cookie_value(value_), std::invalid_argument);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /// @brief Swaps this instance with `other`.
  void swap(Set_cookie& other)
  {
    name_.swap(other.name_);
    value_.swap(other.value_);
    expires_.swap(other.expires_);
    max_age_.swap(other.max_age_);
    domain_.swap(other.domain_);
    path_.swap(other.path_);
    std::swap(is_secure_, other.is_secure_);
    std::swap(is_http_only_, other.is_http_only_);
    same_site_.swap(other.same_site_);
  }

  /// @see Header::to_header().
  std::unique_ptr<Header> to_header() const override
  {
    return std::make_unique<Set_cookie>(*this);
  }

  /// @see Header::field_name().
  const std::string& field_name() const override
  {
    static const std::string result{"Set-Cookie"};
    return result;
  }

  /// @see Header::to_string().
  std::string to_string() const override
  {
    std::string result;
    result.append(name_).append("=").append(value_);
    if (expires_)
      result.append("; ").append("Expires=").append(expires_->to_rfc7231());
    if (max_age_)
      result.append("; ").append("Max-Age=").append(std::to_string(*max_age_));
    if (domain_)
      result.append("; ").append("Domain=").append(*domain_);
    if (path_)
      result.append("; ").append("Path=").append(*path_);
    if (is_secure_)
      result.append("; ").append("Secure");
    if (is_http_only_)
      result.append("; ").append("HttpOnly");
    if (same_site_)
      result.append("; ").append("SameSite=").append(http::to_string(*same_site_));
    return result;
  }

  /**
   * @returns The name of cookie.
   *
   * @see set_name().
   */
  const std::string& name() const noexcept
  {
    return name_;
  }

  /**
   * @brief Sets the name of cookie.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `is_valid_cookie_name(name)`.
   *
   * @see name().
   */
  void set_name(std::string name)
  {
    DMITIGR_REQUIRE(is_valid_cookie_name(name), std::invalid_argument);
    require_consistency<std::invalid_argument>(name, is_secure_, domain_, path_);

    name_ = std::move(name);

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns The value of cookie.
   *
   * @see set_value().
   */
  const std::string& value() const noexcept
  {
    return value_;
  }

  /**
   * @brief Sets the value of cookie.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `is_valid_cookie_value(value)`.
   *
   * @see value().
   */
  void set_value(std::string value)
  {
    DMITIGR_REQUIRE(is_valid_cookie_value(value), std::invalid_argument);

    value_ = std::move(value);

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns The value of "Expires" attribute of cookie.
   *
   * @see set_expires().
   */
  const std::optional<dt::Timestamp>& expires() const noexcept
  {
    return expires_;
  }

  /**
   * @brief Sets the "Expires" attribute of cookie.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  void set_expires(std::optional<dt::Timestamp> ts)
  {
    expires_ = std::move(ts);

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @overload
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @see Date.
   */
  void set_expires(const std::string_view input)
  {
    expires_ = dt::Timestamp::from_rfc7231(input);

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns The value of "MaxAge" attribute of cookie.
   */
  std::optional<int> max_age() const noexcept
  {
    return max_age_;
  }

  /**
   * @brief Sets the "Max-Age" attribute of cookie.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  void set_max_age(std::optional<int> ma)
  {
    max_age_ = ma;

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns The value of "Domain" attribute of cookie.
   */
  const std::optional<std::string>& domain() const noexcept
  {
    return domain_;
  }

  /**
   * @brief Sets the "Domain" attribute of cookie.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  void set_domain(std::optional<std::string> domain)
  {
    require_consistency<std::invalid_argument>(name_, is_secure_, domain, path_);

    domain_ = std::move(domain);

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns The value of "Path" attribute of cookie.
   */
  const std::optional<std::string>& path() const noexcept
  {
    return path_;
  }

  /**
   * @brief Sets the "Path" attribute of cookie.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  void set_path(std::optional<std::string> path)
  {
    require_consistency<std::invalid_argument>(name_, is_secure_, domain_, path);

    path_ = std::move(path);

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns `true` if the "Secure" attribute is presents in cookie, or
   * `false` otherwise.
   */
  bool is_secure() const noexcept
  {
    return is_secure_;
  }

  /**
   * @brief Sets the "Secure" attribute of cookie.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  void set_secure(const bool secure)
  {
    require_consistency<std::invalid_argument>(name_, secure, domain_, path_);

    is_secure_ = secure;

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns `true` if the "HttpOnly" attribute is presents in cookie, or
   * `false` otherwise.
   */
  bool is_http_only() const noexcept
  {
    return is_http_only_;
  }

  /**
   * @brief Sets the "HttpOnly" attribute of cookie.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  void set_http_only(const bool http_only)
  {
    is_http_only_ = http_only;

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @return The value of "SameSite" attribute of cookie.
   */
  std::optional<Same_site> same_site() const noexcept
  {
    return same_site_;
  }

  /**
   * @brief Sets the "SameSite" attribute of cookie.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  void set_same_site(std::optional<Same_site> same_site)
  {
    same_site_ = std::move(same_site);

    DMITIGR_ASSERT(is_invariant_ok());
  }

private:
  std::string name_;
  std::string value_;
  std::optional<dt::Timestamp> expires_;
  std::optional<int> max_age_{};
  std::optional<std::string> domain_;
  std::optional<std::string> path_;
  bool is_secure_{};
  bool is_http_only_{};
  std::optional<Same_site> same_site_{};

  // ===========================================================================

  bool is_invariant_ok() const
  {
    return is_valid_cookie_name(name_) && is_valid_cookie_value(value_) &&
      !requirement_violation_details(name_, is_secure_, domain_, path_);
  }

  // ===========================================================================

  /**
   * @internal
   *
   * @returns The string literal with a requirement violation info, or
   * `nullptr` if the requirement met.
   */
  static const char* requirement_violation_details(const std::string& name, const bool is_secure,
    const std::optional<std::string>& domain, const std::optional<std::string>& path)
  {
    if (str::is_begins_with(name, "__Secure-")) {
      if (!is_secure)
        return "cookies with name starting __Secure- must be set with \"secure\" flag";
    }

    if (str::is_begins_with(name, "__Host-")) {
      if (!is_secure)
        return "cookies with name starting __Host- must be set with \"secure\" flag";
      if (domain)
        return "cookies with name starting __Host- must not have a domain specified";
      if (path != "/")
        return "cookies with name starting __Host- must have path \"/\"";
    }

    return nullptr;
  }

  /**
   * @internal
   *
   * @brief Checks the consistency of the arguments.
   *
   * @throws The instance of the specified type `E`
   * if the specified arguments are not consistent.
   */
  template<class E>
  static void require_consistency(const std::string& name, const bool is_secure,
    const std::optional<std::string>& domain, const std::optional<std::string>& path)
  {
    const char* const details = requirement_violation_details(name, is_secure, domain, path);
    DMITIGR_REQUIRE(!details, E);
  }
};

} // namespace dmitigr::http

#endif  // DMITIGR_HTTP_SET_COOKIE_HPP
