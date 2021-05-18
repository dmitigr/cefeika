// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_EXCEPTIONS_HPP
#define DMITIGR_HTTP_EXCEPTIONS_HPP

#include "std_system_error.hpp"

namespace dmitigr::http {

/**
 * @brief The base class of exceptions thrown on a server side.
 */
class Server_exception final : public std::system_error {
private:
  /**
   * @brief The constructor.
   */
  Server_exception(const std::error_code ec)
    : system_error{ec}
  {}

  /**
   * @overload
   */
  Server_exception(const std::error_code ec, const std::string& what)
    : system_error{ec, what}
  {}
};

} // namespace dmitigr::http

#endif  // DMITIGR_HTTP_EXCEPTIONS_HPP
