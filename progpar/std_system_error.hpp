// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#ifndef DMITIGR_PROGPAR_STD_SYSTEM_ERROR_HPP
#define DMITIGR_PROGPAR_STD_SYSTEM_ERROR_HPP

#include "errc.hpp"

#include <string>
#include <system_error>

namespace dmitigr::progpar {

/**
 * @brief A category of runtime server errors.
 *
 * @see Exception.
 */
class Error_category final : public std::error_category {
public:
  /// @returns The literal `dmitigr_progpar_error`.
  const char* name() const noexcept override
  {
    return "dmitigr_progpar_error";
  }

  /**
   * @returns The string that describes the error condition denoted by `ev`.
   *
   * @par Requires
   * `ev` must corresponds to the value of Server_errc.
   *
   * @remarks The caller should not rely on the return value as it is a
   * subject to change.
   */
  std::string message(const int ev) const override
  {
    return std::string{name()}.append(": ").append(str(static_cast<Errc>(ev)));
  }
};

/// @returns The reference to the instance of type Error_category.
inline const Error_category& error_category() noexcept
{
  static const Error_category result;
  return result;
}

/// @returns `std::error_condition(int(errc), error_category())`
inline std::error_condition make_error_condition(const Errc errc) noexcept
{
  return std::error_condition{static_cast<int>(errc), error_category()};
}

} // namespace dmitigr::progpar

namespace std {

/// The full specialization for integration with `<system_error>`.
template<> struct is_error_condition_enum<dmitigr::progpar::Errc> final : true_type {};

} // namespace std

#endif  // DMITIGR_PROGPAR_STD_SYSTEM_ERROR_HPP
