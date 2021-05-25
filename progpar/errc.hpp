// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or progpar.hpp

#ifndef DMITIGR_PROGPAR_ERRC_HPP
#define DMITIGR_PROGPAR_ERRC_HPP

namespace dmitigr::progpar {

/// Error conditions.
enum class Errc {
  /// Option isn't specified.
  option_not_specified = 1,

  /// Option requires a value.
  option_without_value,

  /// Option requires a non-empty value.
  option_with_empty_value,

  /// Option doesn't need a value.
  option_with_value,
};

/**
 * @returns The human-readable literal of `value`, or `nullptr`
 * if `value` does not corresponds to any value defined by Errc.
 */
constexpr const char* str(const Errc value) noexcept
{
  switch (value) {
  case Errc::option_not_specified:
    return "option is not specified";
  case Errc::option_without_value:
    return "option requires a value";
  case Errc::option_with_empty_value:
    return "option requires a non-empty value";
  case Errc::option_with_value:
    return "option does not need a value";
  }
  return nullptr;
}

} // namespace dmitigr::progpar

#endif  // DMITIGR_PROGPAR_ERRC_HPP
