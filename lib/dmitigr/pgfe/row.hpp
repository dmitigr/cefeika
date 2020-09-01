// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#ifndef DMITIGR_PGFE_ROW_HPP
#define DMITIGR_PGFE_ROW_HPP

#include "dmitigr/pgfe/compositional.hpp"
#include "dmitigr/pgfe/data.hpp"
#include "dmitigr/pgfe/response.hpp"

namespace dmitigr::pgfe {

/**
 * @ingroup main
 *
 * @brief A row produced by a PostgreSQL server.
 */
class Row : public Response, public Compositional {
public:
  /// @returns The information about this row.
  virtual const Row_info& info() const noexcept = 0;

  /// @see Message::is_valid().
  bool is_valid() const noexcept override
  {
    throw "not implemented";
  }

  /**
   * @returns The field data of this row, or invalid instance if NULL.
   *
   * @param index - see Compositional.
   *
   * @par Requires
   * `(index < field_count())`.
   */
  virtual Data_view data(std::size_t index = 0) const = 0;

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
  virtual Data_view data(const std::string& name, std::size_t offset = 0) const = 0;

private:
  friend detail::iRow;

  Row() = default;

  virtual bool is_invariant_ok() const
  {
    return Compositional::is_invariant_ok();
  }
};

} // namespace dmitigr::pgfe

#ifdef DMITIGR_PGFE_HEADER_ONLY
#include "dmitigr/pgfe/row.cpp"
#endif

#endif  // DMITIGR_PGFE_ROW_HPP
