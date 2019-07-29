// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or mulf.hpp

#ifndef DMITIGR_MULF_FORM_DATA_HPP
#define DMITIGR_MULF_FORM_DATA_HPP

#include "dmitigr/mulf/dll.hpp"
#include "dmitigr/mulf/types_fwd.hpp"

#include <cstddef>
#include <memory>
#include <optional>
#include <string>

namespace dmitigr::mulf {

/**
 * @brief An entry of multipart/form-data.
 */
class Form_data_entry {
public:
  /**
   * @brief The destructor.
   */
  virtual ~Form_data_entry() = default;

  /**
   * @returns The name of the entry.
   *
   * @see set_name().
   */
  virtual const std::string& name() const = 0;

  /**
   * @brief Sets the name of the entry.
   *
   * @par Requires
   * `!name.empty()`.
   *
   * @see name().
   */
  virtual void set_name(std::string name) = 0;

  /**
   * @returns The filename of the entry.
   *
   * @see set_filename().
   */
  virtual const std::optional<std::string>& filename() const = 0;

  /**
   * @brief Sets the filename of the entry.
   *
   * @par Requires
   * `(!filename || !filename->empty())`.
   *
   * @see filename().
   */
  virtual void set_filename(std::optional<std::string> filename) = 0;

  /**
   * @returns The content type of the entry.
   *
   * @see set_content_type().
   */
  virtual const std::optional<std::string>& content_type() const = 0;

  /**
   * @brief Sets the content type of the entry.
   *
   * @par Requires
   * `(!content_type || !content_type->empty())`.
   *
   * @see content_type().
   */
  virtual void set_content_type(std::optional<std::string> content_type) = 0;

  /**
   * @returns The charset of the entry.
   *
   * @see set_charset().
   */
  virtual const std::optional<std::string>& charset() const = 0;

  /**
   * @brief Sets the charset of the entry.
   *
   * @par Requires
   * `(!charset || !charset->empty())`.
   *
   * @see charset().
   */
  virtual void set_charset(std::optional<std::string> charset) = 0;

  /**
   * @returns The content of the entry.
   *
   * @see set_content().
   */
  virtual std::optional<std::string_view> content() const = 0;

  /**
   * @brief Sets the content of the entry.
   *
   * @par Requires
   * `(!view || !view->empty())`.
   *
   * @see content().
   */
  virtual void set_content(std::optional<std::string_view> view) = 0;

  /**
   * @overload
   *
   * @par Requires
   * `(!content || !content->empty())`.
   *
   * @see content().
   */
  virtual void set_content(std::optional<std::string> content) = 0;

private:
  friend detail::iForm_data_entry;

  Form_data_entry() = default;
};

/**
 * @brief A parsed multipart/form-data.
 *
 * @remarks Since several entries can be named equally, `offset` can be
 * specified as the starting lookup index in the corresponding methods.
 *
 */
class Form_data {
public:
  /** The alias of Form_data_entry. */
  using Entry = Form_data_entry;

  /**
   * @brief The destructor.
   */
  virtual ~Form_data() = default;

  /// @name Constructors
  /// @{

  /**
   * @brief Constructs the object by parsing the multipart/form-data.
   *
   * @param data - the unparsed multipart/form-data;
   * @param boundary - the boundary of multipart/form-data.
   *
   * @remarks The `data` will be used as a storage area to avoid
   * copying the content of the entries (which are can be huge).
   */
  static DMITIGR_MULF_API std::unique_ptr<Form_data> make(std::string data, const std::string& boundary);

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
   *
   * @returns `entry(entry_index_throw(name, offset))`.
   */
  virtual const Entry* entry(std::string_view name, std::size_t offset = 0) const = 0;

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

private:
  friend detail::iForm_data;

  Form_data() = default;
};

} // namespace dmitigr::mulf

#ifdef DMITIGR_MULF_HEADER_ONLY
#include "dmitigr/mulf/form_data.cpp"
#endif

#endif // DMITIGR_MULF_FORM_DATA_HPP
