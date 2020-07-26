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
