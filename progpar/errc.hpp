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

  /// Option doesn't need an argument.
  option_with_argument,

  /// Option requires an argument.
  option_without_argument
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
  case Errc::option_with_argument:
    return "option does not need an argument";
  case Errc::option_without_argument:
    return "option requires an argument";
  }
  return nullptr;
}

} // namespace dmitigr::progpar

#endif  // DMITIGR_PROGPAR_ERRC_HPP
