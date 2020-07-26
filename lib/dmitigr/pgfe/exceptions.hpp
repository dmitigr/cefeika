// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#ifndef DMITIGR_PGFE_EXCEPTIONS_HPP
#define DMITIGR_PGFE_EXCEPTIONS_HPP

#include "dmitigr/pgfe/error.hpp"
#include "dmitigr/pgfe/std_system_error.hpp"
#include "dmitigr/pgfe/types_fwd.hpp"
#include <dmitigr/base/debug.hpp>

#include <memory>

namespace dmitigr::pgfe {

/**
 * @ingroup errors
 *
 * @brief The base class of exceptions thrown on a client side.
 */
class Client_exception : public std::system_error {
public:
  /**
   * @brief The constructor.
   */
  explicit Client_exception(const Client_errc errc)
    : system_error(errc)
  {}

  /**
   * @overload
   */
  Client_exception(const Client_errc errc, const std::string& what)
    : system_error(errc, what)
  {}
};

/**
 * @ingroup errors
 *
 * @brief Denotes an insufficient array dimensionality.
 */
class Insufficient_array_dimensionality final : public Client_exception {
public:
  /// The constructor.
  Insufficient_array_dimensionality()
    : Client_exception{Client_errc::insufficient_array_dimensionality}
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
  Excessive_array_dimensionality()
    : Client_exception{Client_errc::excessive_array_dimensionality}
  {}
};

/**
 * @ingroup errors
 *
 * @brief Denotes a malformed array literal.
 */
class Malformed_array_literal final : public Client_exception {
public:
  /// The constructor.
  Malformed_array_literal()
    : Client_exception{Client_errc::malformed_array_literal}
  {}
};

/**
 * @ingroup errors
 *
 * @brief Denotes an usage of container with improper type of elements.
 */
class Improper_value_type_of_container final : public Client_exception {
public:
  /// The constructor.
  Improper_value_type_of_container()
    : Client_exception{Client_errc::improper_value_type_of_container}
  {}
};

/**
 * @ingroup errors
 *
 * @brief Denotes a timed out operation.
 */
class Timed_out final : public Client_exception {
public:
  /// The constructor.
  Timed_out()
    : Client_exception{Client_errc::timed_out}
  {}
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
  /**
   * @brief The constructor.
   */
  explicit Server_exception(std::shared_ptr<Error> error)
    : system_error(error ? error->code() : std::error_code{})
    , error_(std::move(error))
  {
    DMITIGR_REQUIRE(error_, std::invalid_argument, "invalid Error instance");
  }

  /**
   * @returns The error response (aka error report).
   */
  const Error* error() const noexcept
  {
    return error_.get();
  }

private:
  std::shared_ptr<Error> error_;
};

} // namespace dmitigr::pgfe

#endif  // DMITIGR_PGFE_EXCEPTIONS_HPP
