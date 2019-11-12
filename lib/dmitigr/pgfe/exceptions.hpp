// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#ifndef DMITIGR_PGFE_EXCEPTIONS_HPP
#define DMITIGR_PGFE_EXCEPTIONS_HPP

#include "dmitigr/pgfe/std_system_error.hpp"
#include "dmitigr/pgfe/types_fwd.hpp"

namespace dmitigr::pgfe {

/**
 * @ingroup errors
 *
 * @brief The base class of exceptions thrown on a client side.
 */
class Client_exception : public std::system_error {
private:
  friend detail::iClient_exception;

  /**
   * @brief The constructor.
   */
  Client_exception(std::error_code ec) : system_error(ec)
  {}

  /**
   * @overload
   */
  Client_exception(std::error_code ec, const std::string& what) : system_error(ec, what)
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
   * @returns The error response (aka error report).
   */
  virtual const Error* error() const noexcept = 0;

private:
  friend detail::iServer_exception;

  Server_exception(std::error_code ec) : system_error(ec)
  {}

  Server_exception(std::error_code ec, const std::string& what) : system_error(ec, what)
  {}
};

} // namespace dmitigr::pgfe

#include "dmitigr/pgfe/exceptions.cpp"

#endif  // DMITIGR_PGFE_EXCEPTIONS_HPP
