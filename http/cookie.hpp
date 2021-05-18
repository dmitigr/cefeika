// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_COOKIE_HPP
#define DMITIGR_HTTP_COOKIE_HPP

#include "header.hpp"
#include "syntax.hpp"
#include "types_fwd.hpp"
#include "../assert.hpp"

#include <algorithm>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace dmitigr::http {

/**
 * @brief A HTTP Cookie header entry.
 */
class Cookie_entry final {
public:
  /**
   * @brief The constructor.
   *
   * @par Requires
   * `(is_valid_cookie_name(name) && is_valid_cookie_value(value))`.
   */
  explicit Cookie_entry(std::string name, std::string value = {})
    : name_{std::move(name)}
    , value_{std::move(value)}
  {
    DMITIGR_CHECK_ARG(is_valid_cookie_name(name_));
    DMITIGR_CHECK_ARG(is_valid_cookie_value(value_));
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns The entry name.
   */
  const std::string& name() const noexcept
  {
    return name_;
  }

  /**
   * @brief Sets the name of entry.
   *
   * @par Requires
   * `is_valid_cookie_name(name)`.
   */
  void set_name(std::string name)
  {
    DMITIGR_CHECK_ARG(is_valid_cookie_name(name));
    name_ = std::move(name);
  }

  /**
   * @returns The entry value.
   */
  const std::string& value() const noexcept
  {
    return value_;
  }

  /**
   * @brief Sets the value of entry.
   *
   * @par Requires
   * `is_valid_cookie_value(value)`.
   */
  void set_value(std::string value)
  {
    DMITIGR_CHECK_ARG(is_valid_cookie_value(value));
    value_ = std::move(value);
  }

private:
  friend Cookie;

  std::string name_;
  std::string value_;

  Cookie_entry() = default; // constructs invalid object!

  bool is_invariant_ok() const
  {
    return is_valid_cookie_name(name_) && is_valid_cookie_value(value_);
  }
};

/**
 * @ingroup headers
 *
 * @brief A HTTP Cookie header.
 *
 * @remarks Since several entries can be named equally, parameter `offset`
 * can be specified as the starting lookup index in the corresponding methods.
 */
class Cookie final : public Header {
public:
  /**
   * @brief The alias of Cookie_entry.
   */
  using Entry = Cookie_entry;

  /**
   * @brief Constructs the object by parsing the `input`.
   *
   * Examples of valid input are:
   *
   *   1. name=value
   *   2. name=value; name2=value2; name3=value3
   */
  explicit Cookie(const std::string_view input = {})
  {
    /*
     * According to: https://tools.ietf.org/html/rfc6265#section-5.4
     * See also: https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cookie
     */

    if (input.empty())
      return;

    enum { name, value, semicolon } state = name;

    const auto append_invalid_entry = [this]{ entries_.emplace_back(Entry{}); };

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
          DMITIGR_ASSERT(entries_.back().is_invariant_ok()); // check the entry
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
  }

  /// @see Header::to_header().
  std::unique_ptr<Header> to_header() const override
  {
    return std::make_unique<Cookie>(*this);
  }

  /// @see Header::field_name().
  const std::string& field_name() const override
  {
    static const std::string result{"Cookie"};
    return result;
  }

  /// @see Header::to_string().
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

  /**
   * @returns The number of entries.
   */
  std::size_t entry_count() const
  {
    return entries_.size();
  }

  /**
   * @returns The entry index if `has_entry(name, offset)`, or
   * `std::nullopt` otherwise.
   */
  std::optional<std::size_t> entry_index(const std::string_view name, const std::size_t offset = 0) const
  {
    if (offset < entry_count()) {
      const auto b = cbegin(entries_);
      const auto e = cend(entries_);
      const auto i = std::find_if(b + offset, e, [&](const auto& entry) { return entry.name() == name; });
      return (i != e) ? std::make_optional(i - b) : std::nullopt;
    } else
      return std::nullopt;
  }

  /**
   * @returns The entry index.
   *
   * @par Requires
   * `has_entry(name, offset)`.
   */
  std::size_t entry_index_throw(const std::string_view name, const std::size_t offset = 0) const
  {
    const auto result = entry_index(name, offset);
    DMITIGR_ASSERT(result);
    return *result;
  }

  /**
   * @returns The entry.
   *
   * @par Requires
   * `(index < entry_count())`.
   */
  const Entry& entry(const std::size_t index) const
  {
    DMITIGR_CHECK_RANGE(index < entry_count());
    return entries_[index];
  }

  /**
   * @overload
   */
  Entry& entry(const std::size_t index)
  {
    return const_cast<Entry&>(static_cast<const Cookie*>(this)->entry(index));
  }

  /**
   * @overload
   *
   * @par Requires
   * `has_entry(name, offset)`.
   */
  const Entry& entry(const std::string_view name, const std::size_t offset = 0) const
  {
    const auto index = entry_index_throw(name, offset);
    return entries_[index];
  }

  /**
   * @overload
   */
  Entry& entry(const std::string_view name, const std::size_t offset = 0)
  {
    return const_cast<Entry&>(static_cast<const Cookie*>(this)->entry(name, offset));
  }

  /**
   * @returns `true` if this instance has the entry with the specified `name`,
   * or `false` otherwise.
   */
  bool has_entry(const std::string_view name, const std::size_t offset = 0) const
  {
    return static_cast<bool>(entry_index(name, offset));
  }

  /**
   * @returns `(entry_count() > 0)`
   */
  bool has_entries() const
  {
    return !entries_.empty();
  }

  /**
   * @brief Appends the entry.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(is_valid_cookie_name(name) && is_valid_cookie_value(value))`.
   */
  void append_entry(std::string name, std::string value)
  {
    entries_.emplace_back(std::move(name), std::move(value));
  }

  /**
   * @brief Removes entry.
   *
   * @par Requires
   * `(index < entry_count())`.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  void remove_entry(const std::size_t index)
  {
    DMITIGR_CHECK_RANGE(index < entry_count());
    entries_.erase(cbegin(entries_) + index);
  }

  /**
   * @overload
   *
   * @par Effects
   * `!has_entry(name, offset)`.
   */
  void remove_entry(const std::string_view name, const std::size_t offset = 0)
  {
    if (const auto index = entry_index(name, offset))
      entries_.erase(cbegin(entries_) + *index);

    DMITIGR_ASSERT(!has_entry(name, offset));
  }

private:
  std::vector<Entry> entries_;
};

} // namespace dmitigr::http

#endif  // DMITIGR_HTTP_COOKIE_HPP
