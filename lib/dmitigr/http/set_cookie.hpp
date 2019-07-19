// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_SET_COOKIE_HPP
#define DMITIGR_HTTP_SET_COOKIE_HPP

#include "dmitigr/http/dll.hpp"
#include "dmitigr/http/header.hpp"
#include "dmitigr/http/types_fwd.hpp"

#include <dmitigr/dt/timestamp.hpp>

#include <memory>
#include <optional>
#include <string>

namespace dmitigr::http {

/**
 * @ingroup headers
 *
 * @brief Defines an abstraction of the HTTP Set-Cookie header.
 */
class Set_cookie : public Header {
public:
  /**
   * @brief The destructor.
   */
  virtual ~Set_cookie() = default;

  /// @name Constructors
  /// @{

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
  static DMITIGR_HTTP_API std::unique_ptr<Set_cookie> make(std::string_view input);

  /**
   * @overload
   *
   * @par Requires
   * `(is_valid_cookie_name(name) && is_valid_cookie_value(value))`.
   */
  static DMITIGR_HTTP_API std::unique_ptr<Set_cookie> make(std::string name, std::string value);

  /**
   * @returns The copy of this instance.
   */
  virtual std::unique_ptr<Set_cookie> to_set_cookie() const = 0;

  /// @}

  /**
   * @returns The name of cookie.
   *
   * @see set_name().
   */
  virtual const std::string& name() const = 0;

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
  virtual void set_name(std::string name) = 0;

  /**
   * @returns The value of cookie.
   *
   * @see set_value().
   */
  virtual const std::string& value() const = 0;

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
  virtual void set_value(std::string value) = 0;

  /**
   * @returns The value of "Expires" attribute of cookie.
   *
   * @see set_expires().
   */
  virtual const dt::Timestamp* expires() const = 0;

  /**
   * @brief Sets the "Expires" attribute of cookie.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  virtual void set_expires(const dt::Timestamp* const ts) = 0;

  /**
   * @overload
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @see Date.
   */
  virtual void set_expires(std::string_view input) = 0;

  /**
   * @returns The value of "MaxAge" attribute of cookie.
   */
  virtual std::optional<int> max_age() const = 0;

  /**
   * @brief Sets the "Max-Age" attribute of cookie.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  virtual void set_max_age(std::optional<int> ma) = 0;

  /**
   * @returns The value of "Domain" attribute of cookie.
   */
  virtual const std::optional<std::string>& domain() const = 0;

  /**
   * @brief Sets the "Domain" attribute of cookie.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  virtual void set_domain(std::optional<std::string> domain) = 0;

  /**
   * @returns The value of "Path" attribute of cookie.
   */
  virtual const std::optional<std::string>& path() const = 0;

  /**
   * @brief Sets the "Path" attribute of cookie.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  virtual void set_path(std::optional<std::string> path) = 0;

  /**
   * @returns `true` if the "Secure" attribute is presents in cookie, or
   * `false` otherwise.
   */
  virtual bool is_secure() const = 0;

  /**
   * @brief Sets the "Secure" attribute of cookie.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  virtual void set_secure(bool secure) = 0;

  /**
   * @returns `true` if the "HttpOnly" attribute is presents in cookie, or
   * `false` otherwise.
   */
  virtual bool is_http_only() const = 0;

  /**
   * @brief Sets the "HttpOnly" attribute of cookie.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  virtual void set_http_only(bool http_only) = 0;

  /**
   * @return The value of "SameSite" attribute of cookie.
   */
  virtual std::optional<Same_site> same_site() const = 0;

  /**
   * @brief Sets the "SameSite" attribute of cookie.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  virtual void set_same_site(std::optional<Same_site> ss) = 0;
private:
  friend detail::iSet_cookie;

  Set_cookie() = default;
};

} // namespace dmitigr::http

#ifdef DMITIGR_HTTP_HEADER_ONLY
#include "dmitigr/http/set_cookie.cpp"
#endif

#endif  // DMITIGR_HTTP_SET_COOKIE_HPP
