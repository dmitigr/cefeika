// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#ifndef DMITIGR_PGFE_ROW_HPP
#define DMITIGR_PGFE_ROW_HPP

#include "dmitigr/pgfe/compositional.hpp"
#include "dmitigr/pgfe/data.hpp"
#include "dmitigr/pgfe/row_info.hpp"

#include <cassert>

namespace dmitigr::pgfe {

/**
 * @ingroup main
 *
 * @brief A row produced by a PostgreSQL server.
 */
class Row final : public Response, public Compositional {
public:
  /// Default-constructible.
  Row() = default;

  /// The constructor.
  template<typename ... Types>
  explicit Row(Types&& ... args)
    : info_{std::forward<Types>(args)...}
  {
    assert(is_invariant_ok());
  }

  /// @see Message::is_valid().
  bool is_valid() const noexcept override
  {
    return static_cast<bool>(info_.pq_result_);
  }

  // ---------------------------------------------------------------------------
  // Compositional overridings
  // ---------------------------------------------------------------------------

  std::size_t field_count() const override
  {
    return info_.field_count();
  }

  bool has_fields() const override
  {
    return info_.has_fields();
  }

  const std::string& field_name(const std::size_t index) const override
  {
    return info_.field_name(index);
  }

  std::optional<std::size_t> field_index(const std::string& name, const std::size_t offset = 0) const override
  {
    return info_.field_index(name, offset);
  }

  std::size_t field_index_throw(const std::string& name, std::size_t offset) const override
  {
    return info_.field_index_throw(name, offset);
  }

  bool has_field(const std::string& name, const std::size_t offset = 0) const override
  {
    return info_.has_field(name, offset);
  }

  /// @returns The information about this row.
  const Row_info& info() const noexcept
  {
    return info_;
  }

  /**
   * @returns The field data of this row, or invalid instance if NULL.
   *
   * @param index - see Compositional.
   *
   * @par Requires
   * `(index < field_count())`.
   */
  Data_view data(std::size_t index = 0) const
  {
    assert(index < field_count());
    return data__(index);
  }

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
  Data_view data(const std::string& name, std::size_t offset = 0) const
  {
    const auto index = field_index_throw(name, offset);
    return data__(index);
  }

private:
  Row_info info_; // has pq_result_

  bool is_invariant_ok() const override
  {
    const bool info_ok = (info_.pq_result_.status() == PGRES_SINGLE_TUPLE);
    return info_ok && Compositional::is_invariant_ok();;
  }

  Data_view data__(const std::size_t index) const noexcept
  {
    constexpr int row{};
    const auto fld = static_cast<int>(index);
    const auto& r = info_.pq_result_;
    if (!r.is_data_null(row, fld))
      return Data_view{r.data_value(row, fld), r.data_size(row, fld), r.field_format(fld)};
    else
      return Data_view{};
  }
};

} // namespace dmitigr::pgfe

#endif  // DMITIGR_PGFE_ROW_HPP
