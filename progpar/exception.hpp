// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or progpar.hpp

#ifndef DMITIGR_PROGPAR_EXCEPTION_HPP
#define DMITIGR_PROGPAR_EXCEPTION_HPP

#include "std_system_error.hpp"

#include <exception>

namespace dmitigr::progpar {

/// The class of exceptions.
class Exception : public std::exception {
public:
  /// The constructor.
  explicit Exception(const Errc errc,
    std::string what, std::string context)
    : condition_{errc}
    , what_holder_{what}
    , context_{std::move(context)}
  {}

  /// @returns The error condition.
  const std::error_condition& condition() const noexcept
  {
    return condition_;
  }

  /// @returns An explanatory string.
  const char* what() const noexcept override
  {
    return what_holder_.what();
  }

  /**
   * @returns An error context depends on condition. For
   * example, it could be a problematic option name.
   */
  const std::string& context() const noexcept
  {
    return context_;
  }

private:
  std::error_condition condition_;
  std::runtime_error what_holder_;
  std::string context_;
};

} // namespace dmitigr::progpar

#endif  // DMITIGR_PROGPAR_EXCEPTION_HPP
