// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#ifndef DMITIGR_PGFE_PARAMETERIZABLE_HPP
#define DMITIGR_PGFE_PARAMETERIZABLE_HPP

#include "dmitigr/pgfe/types_fwd.hpp"

#include <optional>
#include <string>

namespace dmitigr::pgfe {

/**
 * @ingroup main
 *
 * @brief A parameterizable type.
 *
 * @remarks The named parameters follows *after* positional parameters.
 */
class Parameterizable {
public:
  /// The destructor.
  virtual ~Parameterizable() = default;

  /// @returns The number of positional parameters.
  virtual std::size_t positional_parameter_count() const = 0;

  /// @returns The number of named parameters.
  virtual std::size_t named_parameter_count() const = 0;

  /// @returns `(positional_parameter_count() + named_parameter_count())`.
  virtual std::size_t parameter_count() const = 0;

  /**
   * @returns The name of the parameter by the `index`.
   *
   * @par Requires
   * `index` in range `[positional_parameter_count(), parameter_count())`.
   */
  virtual const std::string& parameter_name(std::size_t index) const = 0;

  /// @returns The parameter index if `has_parameter(name)`.
  virtual std::optional<std::size_t> parameter_index(const std::string& name) const = 0;

  /**
   * @returns The parameter index.
   *
   * @par Requires
   * `has_parameter(name)`.
   */
  virtual std::size_t parameter_index_throw(const std::string& name) const = 0;

  /// @returns `true` if this instance has the parameter with the specified `name`.
  virtual bool has_parameter(const std::string& name) const = 0;

  /// @returns `(positional_parameter_count() > 0)`.
  virtual bool has_positional_parameters() const = 0;

  /// @returns `(named_parameter_count() > 0)`.
  virtual bool has_named_parameters() const = 0;

  /// @returns `(parameter_count() > 0)`.
  virtual bool has_parameters() const = 0;

private:
  friend Prepared_statement;
  friend Sql_string;

  Parameterizable() = default;
};

} // namespace dmitigr::pgfe

#ifdef DMITIGR_PGFE_HEADER_ONLY
#include "dmitigr/pgfe/parameterizable.cpp"
#endif

#endif  // DMITIGR_PGFE_PARAMETERIZABLE_HPP
