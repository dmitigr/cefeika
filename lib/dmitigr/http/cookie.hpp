// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_COOKIE_HPP
#define DMITIGR_HTTP_COOKIE_HPP

#include "dmitigr/http/dll.hpp"
#include "dmitigr/http/header.hpp"
#include "dmitigr/http/types_fwd.hpp"

#include <memory>
#include <optional>
#include <string>

namespace dmitigr::http {

/**
 * @brief A HTTP Cookie header entry.
 */
class Cookie_entry {
public:
  /**
   * @returns The entry name.
   */
  virtual const std::string& name() const = 0;

  /**
   * @brief Sets the name of entry.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `is_valid_cookie_name(name)`.
   */
  virtual void set_name(std::string name) = 0;

  /**
   * @returns The entry value.
   */
  virtual const std::string& value() const = 0;

  /**
   * @brief Sets the value of entry.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `is_valid_cookie_value(value)`.
   */
  virtual void set_value(std::string value) = 0;

private:
  friend detail::iCookie_entry;

  Cookie_entry() = default;
};

/**
 * @ingroup headers
 *
 * @brief A HTTP Cookie header.
 *
 * @remarks Since several entries can be named equally, parameter `offset`
 * can be specified as the starting lookup index in the corresponding methods.
 */
class Cookie : public Header {
public:
  /**
   * @brief The alias of Cookie_entry.
   */
  using Entry = Cookie_entry;

  /**
   * @brief The destructor.
   */
  virtual ~Cookie() = default;

  /// @name Constructors
  /// @{

  /**
   * @brief Constructs the object by parsing the `input`.
   *
   * Examples of valid input are:
   *
   *   1. name=value
   *   2. name=value; name2=value2; name3=value3
   */
  static DMITIGR_HTTP_API std::unique_ptr<Cookie> make(std::string_view input = {});

  /**
   * @returns The copy of this instance.
   */
  virtual std::unique_ptr<Cookie> to_cookie() const = 0;

  /// @}

  /**
   * @returns The number of entries.
   */
  virtual std::size_t entry_count() const = 0;

  /**
   * @returns The entry index if `has_entry(name, offset)`, or
   * `std::nullopt` otherwise.
   *
   * @par Requires
   * `(offset < entry_count())`.
   */
  virtual std::optional<std::size_t> entry_index(std::string_view name, std::size_t offset = 0) const = 0;

  /**
   * @returns The entry index.
   *
   * @par Requires
   * `has_entry(name, offset)`.
   */
  virtual std::size_t entry_index_throw(std::string_view name, std::size_t offset = 0) const = 0;

  /**
   * @returns The entry.
   *
   * @par Requires
   * `(index < entry_count())`.
   */
  virtual const Entry* entry(std::size_t index) const = 0;

  /**
   * @overload
   */
  virtual Entry* entry(std::size_t index) = 0;

  /**
   * @overload
   *
   * @par Requires
   * `has_entry(name, offset)`.
   */
  virtual const Entry* entry(std::string_view name, std::size_t offset = 0) const = 0;

  /**
   * @overload
   */
  virtual Entry* entry(std::string_view name, std::size_t offset = 0) = 0;

  /**
   * @returns `true` if this instance has the entry with the specified `name`,
   * or `false` otherwise.
   *
   * @par Requires
   * `(offset < entry_count())`.
   */
  virtual bool has_entry(std::string_view name, std::size_t offset = 0) const = 0;

  /**
   * @returns `(entry_count() > 0)`
   */
  virtual bool has_entries() const = 0;

  /**
   * @brief Appends the entry.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(is_valid_cookie_name(name) && is_valid_cookie_value(value))`.
   */
  virtual void append_entry(std::string name, std::string value) = 0;

  /**
   * @brief Removes entry.
   *
   * @par Requires
   * `(index < entry_count())`.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  virtual void remove_entry(std::size_t index) = 0;

  /**
   * @overload
   *
   * @par Requires
   * `(offset < entry_count())`.
   *
   * @par Effects
   * `!has_parameter(name, offset)`.
   */
  virtual void remove_entry(std::string_view name, std::size_t offset = 0) = 0;

private:
  friend detail::iCookie;

  Cookie() = default;
};

} // namespace dmitigr::http

#ifdef DMITIGR_HTTP_HEADER_ONLY
#include "dmitigr/http/cookie.cpp"
#endif

#endif  // DMITIGR_HTTP_COOKIE_HPP
