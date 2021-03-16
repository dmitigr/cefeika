// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#ifndef DMITIGR_PGFE_EXCEPTIONS_HPP
#define DMITIGR_PGFE_EXCEPTIONS_HPP

#include "dmitigr/pgfe/dll.hpp"
#include "dmitigr/pgfe/error.hpp"
#include "dmitigr/pgfe/std_system_error.hpp"
#include "dmitigr/pgfe/types_fwd.hpp"

#include <memory>

namespace dmitigr::pgfe {

/**
 * @ingroup errors
 *
 * @brief The base class of exceptions thrown on a client side.
 */
class Client_exception : public std::system_error {
public:
  /// The constructor.
  explicit Client_exception(const Client_errc errc)
    : system_error(errc) {}

  /// @overload
  Client_exception(const Client_errc errc, const std::string& what)
    : system_error(errc, what) {}
};

/**
 * @ingroup errors
 *
 * @brief Denotes an insufficient array dimensionality.
 */
class Insufficient_array_dimensionality final : public Client_exception {
public:
  /// The constructor.
  explicit Insufficient_array_dimensionality(const std::string& what = {})
    : Client_exception{Client_errc::insufficient_array_dimensionality, what}
  {}
};

/**
 * @ingroup errors
 *
 * @brief Denotes an excessive array dimensionality.
 */
class Excessive_array_dimensionality final : public Client_exception {
public:
  /// The constructor.
  explicit Excessive_array_dimensionality(const std::string& what = {})
    : Client_exception{Client_errc::excessive_array_dimensionality, what} {}
};

/**
 * @ingroup errors
 *
 * @brief Denotes a malformed array literal.
 */
class Malformed_array_literal final : public Client_exception {
public:
  /// The constructor.
  explicit Malformed_array_literal(const std::string& what = {})
    : Client_exception{Client_errc::malformed_array_literal, what} {}
};

/**
 * @ingroup errors
 *
 * @brief Denotes an usage of container with improper type of elements.
 */
class Improper_value_type_of_container final : public Client_exception {
public:
  /// The constructor.
  explicit Improper_value_type_of_container(const std::string& what = {})
    : Client_exception{Client_errc::improper_value_type_of_container, what} {}
};

/**
 * @ingroup errors
 *
 * @brief Denotes a timed out operation.
 */
class Timed_out final : public Client_exception {
public:
  /// The constructor.
  explicit Timed_out(const std::string& what = {})
    : Client_exception{Client_errc::timed_out, what} {}
};

// -----------------------------------------------------------------------------
// Server_exception
// -----------------------------------------------------------------------------

/**
 * @ingroup errors
 *
 * @brief The base class of exceptions thrown on a server side.
 */
class Server_exception : public std::system_error {
public:
  /// The constructor.
  explicit Server_exception(std::shared_ptr<Error> error)
    : system_error(error ? error->code() : std::error_code{})
    , error_(std::move(error))
  {
    assert(error_);
  }

  /// @returns The error response (aka error report).
  const Error& error() const noexcept
  {
    return *error_;
  }

private:
  std::shared_ptr<Error> error_;
};

} // namespace dmitigr::pgfe

#endif  // DMITIGR_PGFE_EXCEPTIONS_HPP
