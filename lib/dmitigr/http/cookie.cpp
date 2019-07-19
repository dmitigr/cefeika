// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#include "dmitigr/http/cookie.hpp"
#include "dmitigr/http/syntax.hpp"
#include "dmitigr/http/implementation_header.hpp"

#include <dmitigr/util/debug.hpp>

#include <algorithm>
#include <utility>
#include <vector>

namespace dmitigr::http::detail {

/**
 * @brief The Cookie_entry implementation.
 */
class iCookie_entry final : public Cookie_entry {
public:
  /**
   * @brief The constructor.
   */
  explicit iCookie_entry(std::string name, std::string value = {})
    : name_{std::move(name)}
    , value_{std::move(value)}
  {
    DMITIGR_REQUIRE(is_valid_cookie_name(name_) && is_valid_cookie_value(value_), std::invalid_argument);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  const std::string& name() const override
  {
    return name_;
  }

  void set_name(std::string name) override
  {
    DMITIGR_REQUIRE(is_valid_cookie_name(name), std::invalid_argument);

    name_ = std::move(name);
  }

  const std::string& value() const override
  {
    return value_;
  }

  void set_value(std::string value) override
  {
    DMITIGR_REQUIRE(is_valid_cookie_value(value), std::invalid_argument);

    value_ = std::move(value);
  }

private:
  friend iCookie;

  std::string name_;
  std::string value_;

  bool is_invariant_ok() const
  {
    return is_valid_cookie_name(name_) && is_valid_cookie_value(value_);
  }

  iCookie_entry() = default; // constructs invalid object!
};

/**
 * @brief The Cookie implementation.
 */
class iCookie final : public Cookie {
public:
  /**
   * @brief The alias of iCookie_entry.
   */
  using iEntry = iCookie_entry;

  /**
   * @brief See Cookie::make().
   */
  explicit iCookie(const std::string_view input)
  {
    /*
     * According to: https://tools.ietf.org/html/rfc6265#section-5.4
     * See also: https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cookie
     */

    if (input.empty()) {
      DMITIGR_ASSERT(is_invariant_ok());
      return;
    }

    enum { name, value, semicolon } state = name;

    const auto append_invalid_entry = [&]()
    {
      entries_.emplace_back(iEntry{});
    };

    append_invalid_entry();
    std::string* extracted = &entries_.back().name_; // extracting the name first
    for (const auto c : input) {
      switch (state) {
      case name:
        if (c == '=') {
          extracted = &entries_.back().value_; // extracting the value now
          state = value;
          continue; // skip =
        } else if (!detail::rfc6265::is_valid_token_character(c))
          throw std::runtime_error{"dmitigr::http: invalid cookie name"};
        break;

      case value:
        if (c == ';') {
          append_invalid_entry();
          extracted = &entries_.back().name_; // extracting the name now
          state = semicolon;
          continue; // skip ;
        } else if (!detail::rfc6265::is_valid_cookie_octet(c))
          throw std::runtime_error{"dmitigr::http: invalid cookie value"};
        break;

      case semicolon:
        if (c == ' ') {
          state = name;
          continue; // skip space
        } else
          throw std::runtime_error{"dmitigr::http: no space after the semicolon"};
        break;
      }

      *extracted += c;
    }

    if (state != value)
      throw std::runtime_error{"dmitigr::http: invalid cookie string"};

    DMITIGR_ASSERT(is_invariant_ok());
  }

  // ---------------------------------------------------------------------------
  // Header overridings
  // ---------------------------------------------------------------------------

  std::unique_ptr<Header> to_header() const override
  {
    return to_cookie();
  }

  const std::string& field_name() const override
  {
    static const std::string result{"Cookie"};
    return result;
  }

  std::string to_string() const override
  {
    std::string result;
    for (const auto& e : entries_)
      result.append(e.name()).append("=").append(e.value()).append("; ");
    if (!result.empty()) {
      result.pop_back();
      result.pop_back();
    }
    return result;
  }

  // ---------------------------------------------------------------------------
  // Cookie overridings
  // ---------------------------------------------------------------------------

  std::unique_ptr<Cookie> to_cookie() const override
  {
    return std::make_unique<iCookie>(*this);
  }

  std::size_t entry_count() const override
  {
    return entries_.size();
  }

  std::optional<std::size_t> entry_index(const std::string_view name, const std::size_t offset) const override
  {
    DMITIGR_REQUIRE(offset < entry_count(), std::out_of_range);

    const auto b = std::cbegin(entries_);
    const auto e = std::cend(entries_);
    const auto i = std::find_if(b + offset, e, [&](const auto& e) {
      return e.name() == name;
    });
    return (i != e) ? std::make_optional(i - b) : std::nullopt;
  }

  const iEntry* entry(const std::size_t index) const override
  {
    DMITIGR_REQUIRE(index < entry_count(), std::out_of_range);

    return &entries_[index];
  }

  iEntry* entry(const std::size_t index) override
  {
    return const_cast<iEntry*>(static_cast<const iCookie*>(this)->entry(index));
  }

  const iEntry* entry(const std::string_view name, const std::size_t offset) const override
  {
    const auto index = entry_index(name, offset);
    DMITIGR_REQUIRE(index, std::out_of_range);

    return &entries_[*index];
  }

  iEntry* entry(const std::string_view name, const std::size_t offset) override
  {
    return const_cast<iEntry*>(static_cast<const iCookie*>(this)->entry(name, offset));
  }

  bool has_entry(const std::string_view name, const std::size_t offset) const override
  {
    DMITIGR_REQUIRE(offset < entry_count(), std::out_of_range);

    return bool(entry_index(name, offset));
  }

  bool has_entries() const override
  {
    return !entries_.empty();
  }

  void append_entry(std::string name, std::string value) override
  {
    entries_.emplace_back(std::move(name), std::move(value));

    DMITIGR_ASSERT(is_invariant_ok());
  }

  void remove_entry(const std::size_t index) override
  {
    DMITIGR_REQUIRE(index < entry_count(), std::out_of_range);

    entries_.erase(cbegin(entries_) + index);

    DMITIGR_ASSERT(is_invariant_ok());
  }

  void remove_entry(const std::string_view name, const std::size_t offset) override
  {
    const auto index = entry_index(name, offset);
    DMITIGR_REQUIRE(index, std::out_of_range);

    entries_.erase(cbegin(entries_) + *index);

    DMITIGR_ASSERT(is_invariant_ok());
  }

private:
  std::vector<iEntry> entries_;

  bool is_invariant_ok() const
  {
    const bool entries_ok = [&]()
    {
      return std::all_of(cbegin(entries_), cend(entries_), [](const auto& e) {
        return e.is_invariant_ok();
      });
    }();
    return entries_ok;
  }
};

} // namespace dmitigr::http::detail

namespace dmitigr::http {

DMITIGR_HTTP_INLINE std::unique_ptr<Cookie> Cookie::make(const std::string_view input)
{
  using detail::iCookie;
  return std::make_unique<iCookie>(input);
}

} // namespace dmitigr::http

#include "dmitigr/http/implementation_footer.hpp"
