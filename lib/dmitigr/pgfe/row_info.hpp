// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#ifndef DMITIGR_PGFE_ROW_INFO_HPP
#define DMITIGR_PGFE_ROW_INFO_HPP

#include "dmitigr/pgfe/basics.hpp"
#include "dmitigr/pgfe/compositional.hpp"
#include "dmitigr/pgfe/pq.hpp"

#include <cstdint>

namespace dmitigr::pgfe {

/**
 * @ingroup main
 *
 * @brief An information about the rows produced (or that will be produced)
 * by a PostgreSQL server.
 */
class Row_info final : public Compositional {
public:
  /// Default-constructible.
  Row_info() = default;

  /// The constructor.
  explicit DMITIGR_PGFE_API Row_info(detail::pq::Result&& pq_result);

  /// @overload
  DMITIGR_PGFE_API Row_info(detail::pq::Result&& pq_result,
    const std::shared_ptr<std::vector<std::string>>& shared_field_names);

  /// Non copy-constructible.
  Row_info(const Row_info&) = delete;

  /// Move-constructible.
  Row_info(Row_info&&) = default;

  /// Non copy-assignable.
  Row_info& operator=(const Row_info&) = delete;

  /// Non move-assignable.
  Row_info& operator=(Row_info&&) = default;

  /// @name Compositional overridings
  /// @{

  std::size_t size() const noexcept override
  {
    return shared_field_names_->size();
  }

  bool empty() const noexcept override
  {
    return shared_field_names_->empty();
  }

  DMITIGR_PGFE_API const std::string& name_of(const std::size_t index) const noexcept override;

  DMITIGR_PGFE_API std::size_t index_of(const std::string& name, std::size_t offset = 0) const noexcept override;

  /// @}

  /**
   * @returns The object ID of the table if the field at `index` can
   * be identified as a column of a specific table, or `0` otherwise.
   *
   * @param index - see Compositional.
   *
   * @par Requires
   * `(index < size())`.
   */
  DMITIGR_PGFE_API std::uint_fast32_t table_oid(std::size_t index) const;

  /**
   * @overload
   *
   * @param name - see Compositional;
   * @param offset - see Compositional.
   *
   * @par Requires
   * `has_field(name, offset)`.
   *
   * @see has_field().
   */
  std::uint_fast32_t table_oid(const std::string& name, std::size_t offset = 0) const
  {
    return table_oid(index_of(name, offset));
  }

  /**
   * @returns The attribute number of a column if the field at `index` can be
   * identified as the column of a specific table, or `0` otherwise.
   *
   * @param index - see Compositional.
   *
   * @par Requires
   * `(index < size())`.
   *
   * @remarks System columns, such as "oid", have arbitrary negative numbers.
   */
  DMITIGR_PGFE_API std::int_fast32_t table_column_number(std::size_t index) const;

  /**
   * @overload
   *
   * @param name - see Compositional;
   * @param offset - see Compositional.
   *
   * @par Requires
   * `has_field(name, offset)`.
   *
   * @see has_field().
   */
  std::int_fast32_t table_column_number(const std::string& name, std::size_t offset = 0) const
  {
    return table_column_number(index_of(name, offset));
  }

  /**
   * @returns The object identifier of the field's data type.
   *
   * @param index - see Compositional.
   *
   * @par Requires
   * `(index < size())`.
   */
  DMITIGR_PGFE_API std::uint_fast32_t type_oid(std::size_t index) const;

  /**
   * @overload
   *
   * @param name - see Compositional;
   * @param offset - see Compositional.
   *
   * @par Requires
   * `has_field(name, offset)`.
   *
   * @see has_field().
   */
  std::uint_fast32_t type_oid(const std::string& name, std::size_t offset = 0) const
  {
    return type_oid(index_of(name, offset));
  }

  /**
   * @returns
   *   - the number of bytes in the internal representation of the field's data type;
   *   - -1 to indicate "varlena" type;
   *   - -2 to indicate null-terminated C string.
   *
   * @param index - see Compositional.
   *
   * @par Requires
   * `(index < size())`.
   */
  DMITIGR_PGFE_API std::int_fast32_t type_size(std::size_t index) const;

  /**
   * @overload
   *
   * @param name - see Compositional;
   * @param offset - see Compositional.
   *
   * @par Requires
   * `has_field(name, offset)`.
   */
  std::int_fast32_t type_size(const std::string& name, std::size_t offset = 0) const
  {
    return type_size(index_of(name, offset));
  }

  /**
   * @returns
   *   - the type modifier of the field's data;
   *   - -1 to indicate "no information available".
   *
   * @param index - see Compositional.
   *
   * @par Requires
   * `(index < size())`.
   */
  DMITIGR_PGFE_API std::int_fast32_t type_modifier(std::size_t index) const;

  /**
   * @overload
   *
   * @param name - see Compositional;
   * @param offset - see Compositional.
   *
   * @par Requires
   * `has_field(name, offset)`.
   */
  std::int_fast32_t type_modifier(const std::string& name, std::size_t offset = 0) const
  {
    return type_modifier(index_of(name, offset));
  }

  /**
   * @returns The field data format.
   *
   * @param index - see Compositional.
   *
   * @par Requires
   * `(index < size())`.
   */
  DMITIGR_PGFE_API Data_format data_format(std::size_t index) const;

  /**
   * @overload
   *
   * @param name - see Compositional;
   * @param offset - see Compositional.
   *
   * @par Requires
   * `has_field(name, offset)`.
   */
  Data_format data_format(const std::string& name, std::size_t offset = 0) const
  {
    return data_format(index_of(name, offset));
  }

private:
  friend Connection;
  friend Prepared_statement;
  friend Row;

  detail::pq::Result pq_result_;
  std::shared_ptr<std::vector<std::string>> shared_field_names_;

  bool is_invariant_ok() const override
  {
    const bool size_ok = shared_field_names_ &&
      (shared_field_names_->size() == static_cast<std::size_t>(pq_result_.field_count()));

    const bool field_names_ok = [this]
    {
      const std::size_t sz = size();
      for (std::size_t i = 0; i < sz; ++i) {
        if (pq_result_.field_name(static_cast<int>(i)) != (*shared_field_names_)[i])
          return false;
      }
      return true;
    }();

    return size_ok && field_names_ok && Compositional::is_invariant_ok();
  }

  /// @returns The shared vector of field names to use across multiple rows.
  static std::shared_ptr<std::vector<std::string>> make_shared_field_names(const detail::pq::Result& pq_result);
};

} // namespace dmitigr::pgfe

#ifdef DMITIGR_PGFE_HEADER_ONLY
#include "dmitigr/pgfe/row_info.cpp"
#endif

#endif  // DMITIGR_PGFE_ROW_INFO_HPP
