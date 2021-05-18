// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or sqlixx.hpp

#ifndef DMITIGR_SQLIXX_EXCEPTION_HPP
#define DMITIGR_SQLIXX_EXCEPTION_HPP

#include <string>
#include <system_error>

namespace dmitigr::sqlixx {

/// A category of SQLite errors.
class Sqlite_error_category final : public std::error_category {
public:
  /// @returns The literal `dmitigr_sqlixx_sqlite_error`.
  const char* name() const noexcept override
  {
    return "dmitigr_sqlixx_sqlite_error";
  }

  /// @returns The string that describes the error condition denoted by `ev`.
  std::string message(int ev) const override
  {
    return std::string{name()}.append(" ").append(std::to_string(ev));
  }
};

/// The instance of type Sqlite_error_category.
inline Sqlite_error_category sqlite_error_category;

/// An exception.
class Exception final : public std::system_error {
public:
  /// The constructor.
  explicit Exception(const int ev)
    : system_error{ev, sqlite_error_category}
  {}

  /// @overload
  Exception(const int ev, const std::string& what)
    : system_error{ev, sqlite_error_category, what}
  {}
};

} // namespace dmitigr::sqlixx

#endif  // DMITIGR_SQLIXX_EXCEPTION_HPP
