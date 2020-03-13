// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_EXCEPTIONS_HPP
#define DMITIGR_HTTP_EXCEPTIONS_HPP

#include "dmitigr/http/std_system_error.hpp"

namespace dmitigr::http {

/**
 * @brief The base class of exceptions thrown on a server side.
 */
class Server_exception : public std::system_error {
private:
  /**
   * @brief The constructor.
   */
  Server_exception(std::error_code ec);

  /**
   * @overload
   */
  Server_exception(std::error_code ec, const std::string& what);
};

} // namespace dmitigr::http

#ifdef DMITIGR_HTTP_HEADER_ONLY
#include "dmitigr/http/exceptions.cpp"
#endif

#endif  // DMITIGR_HTTP_EXCEPTIONS_HPP
