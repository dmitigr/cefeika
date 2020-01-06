// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#ifndef DMITIGR_WS_HTTP_REQUEST_HPP
#define DMITIGR_WS_HTTP_REQUEST_HPP

#include "dmitigr/ws/types_fwd.hpp"

#include <string>
#include <string_view>

namespace dmitigr::ws {

/**
 * @brief A HTTP request.
 */
class Http_request {
public:
  /**
   * @brief The destructor.
   */
  virtual ~Http_request() = default;

  /**
   * @returns The textual representation of the remote IP address.
   */
  virtual std::string remote_ip_address() const = 0;

  /**
   * @returns The HTTP request method.
   */
  virtual std::string_view method() const = 0;

  /**
   * @returns The HTTP request path.
   */
  virtual std::string_view path() const = 0;

  /**
   * @returns The HTTP request query string.
   */
  virtual std::string_view query_string() const = 0;

  /**
   * @returns The value of HTTP request header named by `name`.
   */
  virtual std::string_view header(std::string_view name) const = 0;

private:
  friend detail::iHttp_request;

  Http_request() = default;
};

} // namespace dmitigr::ws

#ifdef DMITIGR_WS_HEADER_ONLY
#include "dmitigr/ws/http_request.cpp"
#endif

#endif  // DMITIGR_WS_HTTP_REQUEST_HPP
