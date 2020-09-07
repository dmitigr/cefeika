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

  // ---------------------------------------------------------------------------
  // Compositional overridings
  // ---------------------------------------------------------------------------

  /// @see Compositional::field_count().
  std::size_t field_count() const noexcept override
  {
    return shared_field_names_->size();
  }

  /// @see Compositional::has_fields().
  bool has_fields() const noexcept override
  {
    return !shared_field_names_->empty();
  }

  /// @see Compositional::field_name().
  DMITIGR_PGFE_API const std::string& field_name(const std::size_t index) const override;

  /// @see Compositional::field_index().
  std::optional<std::size_t> field_index(const std::string& name, const std::size_t offset = 0) const override
  {
    if (const auto result = field_index__(name, offset); result < field_count())
      return result;
    else
      return std::nullopt;
  }

  /// @see Compositional::field_index_throw().
  DMITIGR_PGFE_API std::size_t field_index_throw(const std::string& name, const std::size_t offset = 0) const override;

  /// @see Compositional::has_field().
  bool has_field(const std::string& name, const std::size_t offset = 0) const override
  {
    return static_cast<bool>(field_index(name, offset));
  }

  /**
   * @returns The object ID of the table if the field at `index` can
   * be identified as a column of a specific table, or `0` otherwise.
   *
   * @param index - see Compositional.
   *
   * @par Requires
   * `(index < field_count())`.
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
    return table_oid(field_index_throw(name, offset));
  }

  /**
   * @returns The attribute number of a column if the field at `index` can be
   * identified as the column of a specific table, or `0` otherwise.
   *
   * @param index - see Compositional.
   *
   * @par Requires
   * `(index < field_count())`.
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
    return table_column_number(field_index_throw(name, offset));
  }

  /**
   * @returns The object identifier of the field's data type.
   *
   * @param index - see Compositional.
   *
   * @par Requires
   * `(index < field_count())`.
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
    return type_oid(field_index_throw(name, offset));
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
   * `(index < field_count())`.
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
    return type_size(field_index_throw(name, offset));
  }

  /**
   * @returns
   *   - the type modifier of the field's data;
   *   - -1 to indicate "no information available".
   *
   * @param index - see Compositional.
   *
   * @par Requires
   * `(index < field_count())`.
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
    return type_modifier(field_index_throw(name, offset));
  }

  /**
   * @returns The field data format.
   *
   * @param index - see Compositional.
   *
   * @par Requires
   * `(index < field_count())`.
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
    return data_format(field_index_throw(name, offset));
  }

private:
  friend Row;
  friend Prepared_statement;
  friend detail::pq_Connection;

  detail::pq::Result pq_result_;
  std::shared_ptr<std::vector<std::string>> shared_field_names_;

  bool is_invariant_ok() const override
  {
    const bool size_ok = shared_field_names_ &&
      (shared_field_names_->size() == static_cast<std::size_t>(pq_result_.field_count()));

    const bool field_names_ok = [this]()
    {
      const std::size_t fc = field_count();
      for (std::size_t i = 0; i < fc; ++i) {
        if (pq_result_.field_name(static_cast<int>(i)) != (*shared_field_names_)[i])
          return false;
      }
      return true;
    }();

    return size_ok && field_names_ok && Compositional::is_invariant_ok();
  }

  /// @returns The index of the field by the given name.
  std::size_t field_index__(const std::string& name, const std::size_t offset) const;

  /// @returns The shared vector of field names to use across multiple rows.
  static std::shared_ptr<std::vector<std::string>> make_shared_field_names(const detail::pq::Result& pq_result);
};

} // namespace dmitigr::pgfe

#ifdef DMITIGR_PGFE_HEADER_ONLY
#include "dmitigr/pgfe/row_info.cpp"
#endif

#endif  // DMITIGR_PGFE_ROW_INFO_HPP
