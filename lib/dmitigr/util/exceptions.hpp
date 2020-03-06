// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or util.hpp

#ifndef DMITIGR_UTIL_EXCEPTIONS_HPP
#define DMITIGR_UTIL_EXCEPTIONS_HPP

#include "dmitigr/util/dll.hpp"

#include <string>
#include <system_error>

namespace dmitigr {

/**
 * @brief An exception thrown on system error.
 */
class Sys_exception final : public std::system_error {
public:
  /**
   * @brief The constructor.
   */
  DMITIGR_UTIL_API explicit Sys_exception(const std::string& func);

  /**
   * @brief Prints the last system error to the standard error.
   */
  static DMITIGR_UTIL_API void report(const char* const func) noexcept;

  /**
   * @returns The last system error code.
   */
  static DMITIGR_UTIL_API int last_error() noexcept;
};

} // namespace dmitigr

#ifdef DMITIGR_UTIL_HEADER_ONLY
#include "dmitigr/util/exceptions.cpp"
#endif

#endif  // DMITIGR_UTIL_EXCEPTIONS_HPP
