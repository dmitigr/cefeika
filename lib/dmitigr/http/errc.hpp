// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_ERRC_HPP
#define DMITIGR_HTTP_ERRC_HPP

namespace dmitigr::http {

/**
 * @brief HTTP informational class of status codes.
 *
 * @see https://tools.ietf.org/html/rfc7231#section-6.2
 */
enum class Server_infoc {
  /// @see https://tools.ietf.org/html/rfc7231#section-6.2.1
  continu = 100,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.2.2
  switching_protocols = 101
};

/**
 * @returns The literal representation of the `errc`, or `nullptr`
 * if `errc` does not corresponds to any value defined by Server_infoc.
 */
constexpr const char* to_literal(const Server_infoc infoc)
{
  switch (infoc) {
  case Server_infoc::continu: return "Continue";
  case Server_infoc::switching_protocols: return "Switching Protocols";
  }
  return nullptr;
}

// -----------------------------------------------------------------------------

/**
 * @brief HTTP successful class of status codes.
 *
 * @see https://tools.ietf.org/html/rfc7231#section-6.3
 */
enum class Server_succ {
  /// @see https://tools.ietf.org/html/rfc7231#section-6.3.1
  ok = 200,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.3.2
  created = 201,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.3.3
  accepted = 202,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.3.4
  non_authoritative_information = 203,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.3.5
  no_content = 204,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.3.6
  reset_content = 205
};

/**
 * @returns The literal representation of the `errc`, or `nullptr`
 * if `errc` does not corresponds to any value defined by Server_succ.
 */
constexpr const char* to_literal(const Server_succ succ)
{
  switch (succ) {
  case Server_succ::ok: return "OK";
  case Server_succ::created: return "Created";
  case Server_succ::accepted: return "Accepted";
  case Server_succ::non_authoritative_information: return "Non-Authoritative Information";
  case Server_succ::no_content: return "No Content";
  case Server_succ::reset_content: return "Reset Content";
  }
  return nullptr;
}

// -----------------------------------------------------------------------------

/**
 * @brief HTTP redirection class of status codes.
 *
 * @see https://tools.ietf.org/html/rfc7231#section-6.4
 */
enum class Server_redirc {
  /// @see https://tools.ietf.org/html/rfc7231#section-6.4.1
  multiple_choices = 300,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.4.2
  moved_permanently = 301,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.4.3
  found = 302,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.4.4
  see_other = 303,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.4.5
  use_proxy = 305,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.4.7
  temporary_redirect = 307
};

/**
 * @returns The literal representation of the `errc`, or `nullptr`
 * if `errc` does not corresponds to any value defined by Server_redirc.
 */
constexpr const char* to_literal(const Server_redirc redirc)
{
  switch (redirc) {
  case Server_redirc::multiple_choices: return "Multiple Choices";
  case Server_redirc::moved_permanently: return "Moved Permanently";
  case Server_redirc::found: return "Found";
  case Server_redirc::see_other: return "See Other";
  case Server_redirc::use_proxy: return "Use Proxy";
  case Server_redirc::temporary_redirect: return "Temporary Redirect";
  }
  return nullptr;
}

// -----------------------------------------------------------------------------

/**
 * @brief HTTP error class of statuc codes.
 *
 * @see https://tools.ietf.org/html/rfc7231#section-6.5
 */
enum class Server_errc {
  /// @see https://tools.ietf.org/html/rfc7231#section-6.5.1
  bad_request = 400,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.5.2
  payment_required = 402,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.5.3
  forbidden = 403,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.5.4
  not_found = 404,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.5.5
  method_not_allowed = 405,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.5.6
  not_acceptable = 406,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.5.7
  request_timeout = 408,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.5.8
  conflict = 409,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.5.9
  gone = 410,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.5.10
  length_required = 411,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.5.11
  payload_too_large = 413,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.5.12
  uri_too_long = 414,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.5.13
  unsupported_media_type = 415,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.5.14
  expectation_failed = 417,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.5.15
  upgrade_required = 426,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.6.1
  internal_server_error = 500,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.6.2
  not_implemented = 501,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.6.3
  bad_gateway = 502,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.6.4
  service_unavailable = 503,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.6.5
  gateway_timeout = 504,

  /// @see https://tools.ietf.org/html/rfc7231#section-6.6.6
  http_version_not_supported = 505
};

/**
 * @returns The literal representation of the `errc`, or `nullptr`
 * if `errc` does not corresponds to any value defined by Server_errc.
 */
constexpr const char* to_literal(const Server_errc errc)
{
  switch (errc) {
  case Server_errc::bad_request: return "Bad Request";
  case Server_errc::payment_required: return "Payment Required";
  case Server_errc::forbidden: return "Forbidden";
  case Server_errc::not_found: return "Not Found";
  case Server_errc::method_not_allowed: return "Method Not Allowed";
  case Server_errc::not_acceptable: return "Not Acceptable";
  case Server_errc::request_timeout: return "Request Timeout";
  case Server_errc::conflict: return "Conflict";
  case Server_errc::gone: return "Gone";
  case Server_errc::length_required: return "Length Required";
  case Server_errc::payload_too_large: return "Payload Too Large";
  case Server_errc::uri_too_long: return "URI Too Long";
  case Server_errc::unsupported_media_type: return "Unsupported Media Type";
  case Server_errc::expectation_failed: return "Expectation Failed";
  case Server_errc::upgrade_required: return "Upgrade Required";
  case Server_errc::internal_server_error: return "Internal Server Error";
  case Server_errc::not_implemented: return "Not Implemented";
  case Server_errc::bad_gateway: return "Bad Gateway";
  case Server_errc::service_unavailable: return "Service Unavailable";
  case Server_errc::gateway_timeout: return "Gateway Timeout";
  case Server_errc::http_version_not_supported: return "HTTP Version Not Supported";
  }
  return nullptr;
}

} // namespace dmitigr::http

#endif  // DMITIGR_HTTP_ERRC_HPP
