// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_ERRC_HPP
#define DMITIGR_HTTP_ERRC_HPP

namespace dmitigr::http {

/**
 * @brief HTTP error codes.
 */
enum class Server_errc {
  /**
   * @brief Indicates that the server cannot or will not process the request
   * due to something that is perceived to be a client error (e.g., malformed
   * request syntax, invalid request message framing, or deceptive request
   * routing).
   *
   * @see https://tools.ietf.org/html/rfc7231#section-6.5.1
   */
   bad_request = 400,

   /**
    * @brief Reserved for future use.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.5.2
    */
   payment_required = 402,

   /**
    * @brief Indicates that the server understood the request but refuses to
    * authorize it.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.5.3
    */
   forbidden = 403,

   /**
    * @brief Indicates that the origin server did not find a current representation
    * for the target resource or is not willing to disclose that one exists.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.5.4
    */
   not_found = 404,

   /**
    * @brief Indicates that the method received in the request-line is known by
    * the origin server but not supported by the target resource.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.5.5
    */
   method_not_allowed = 405,

   /**
    * @brief Indicates that the target resource does not have a current
    * representation that would be acceptable to the user agent, according to
    * the proactive negotiation header fields received in the request, and the
    * server is unwilling to supply a default representation.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.5.6
    */
   not_acceptable = 406,

   /**
    * @brief Indicates that the server did not receive a complete request
    * message within the time that it was prepared to wait.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.5.7
    */
   request_timeout = 408,

   /**
    * @brief Indicates that the request could not be completed due to a conflict
    * with the current state of the target resource.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.5.8
    */
   conflict = 409,

   /**
    * @brief Indicates that access to the target resource is no longer available
    * at the origin server and that this condition is likely to be permanent.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.5.9
    */
   gone = 410,

   /**
    * @brief Indicates that the server refuses to accept the request without a
    * defined `Content-Length`.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.5.10
    */
   length_required = 411,

   /**
    * @brief Indicates that the server is refusing to process a request because
    * the request payload is larger than the server is willing or able to process.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.5.11
    */
   payload_too_large = 413,

   /**
    * @brief Indicates that the server is refusing to service the request because
    * the request-target is longer than the server is willing to interpret.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.5.12
    */
   uri_too_long = 414,

   /**
    * @brief Indicates that the origin server is refusing to service the request
    * because the payload is in a format not supported by this method on the
    * target resource.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.5.13
    */
   unsupported_media_type = 415,

   /**
    * @brief Indicates that the expectation given in the request's `Expect`
    * header field could not be met by at least one of the inbound servers.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.5.14
    */
   expectation_failed = 417,

   /**
    * @brief Indicates that the server refuses to perform the request using the
    * current protocol but might be willing to do so after the client upgrades
    * to a different protocol.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.5.15
    */
   upgrade_required = 426,

   /**
    * @brief Indicates that the server encountered an unexpected condition that
    * prevented it from fulfilling the request.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.6.1
    */
   internal_server_error = 500,

   /**
    * @brief Indicates that the server does not support the functionality
    * required to fulfill the request. This is the appropriate response when
    * the server does not recognize the request method and is not capable of
    * supporting it for any resource.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.6.2
    */
   not_implemented = 501,

   /**
    * @brief Indicates that the server, while acting as a gateway or proxy,
    * received an invalid response from an inbound server it accessed while
    * attempting to fulfill the request.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.6.3
    */
   bad_gateway = 502,

   /**
    * @brief Indicates that the server is currently unable to handle the request
    * due to a temporary overload or scheduled maintenance, which will likely be
    * alleviated after some delay.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.6.4
    */
   service_unavailable = 503,

   /**
    * @brief Indicates that the server, while acting as a gateway or proxy, did
    * not receive a timely response from an upstream server it needed to access
    * in order to complete the request.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.6.5
    */
   gateway_timeout = 504,

   /**
    * @brief Indicates that the server does not support, or refuses to support,
    * the major version of HTTP that was used in the request message.
    *
    * @see https://tools.ietf.org/html/rfc7231#section-6.6.6
    */
   http_version_not_supported = 505
};

/**
 * @returns The literal representation of the `errc`, or `nullptr`
 * if `errc` does not corresponds to any value defined by Server_errc.
 */
constexpr const char* to_literal(const Server_errc errc)
{
  switch (errc) {
  case Server_errc::bad_request: return "bad_request";
  case Server_errc::payment_required: return "payment_required";
  case Server_errc::forbidden: return "forbidden";
  case Server_errc::not_found: return "not_found";
  case Server_errc::method_not_allowed: return "method_not_allowed";
  case Server_errc::not_acceptable: return "not_acceptable";
  case Server_errc::request_timeout: return "request_timeout";
  case Server_errc::conflict: return "conflict";
  case Server_errc::gone: return "gone";
  case Server_errc::length_required: return "length_required";
  case Server_errc::payload_too_large: return "payload_too_large";
  case Server_errc::uri_too_long: return "uri_too_long";
  case Server_errc::unsupported_media_type: return "unsupported_media_type";
  case Server_errc::expectation_failed: return "expectation_failed";
  case Server_errc::upgrade_required: return "upgrade_required";
  case Server_errc::internal_server_error: return "internal_server_error";
  case Server_errc::not_implemented: return "not_implemented";
  case Server_errc::bad_gateway: return "bad_gateway";
  case Server_errc::service_unavailable: return "service_unavailable";
  case Server_errc::gateway_timeout: return "gateway_timeout";
  case Server_errc::http_version_not_supported: return "http_version_not_supported";
  }
  return nullptr;
}

} // namespace dmitigr::http

#endif  // DMITIGR_HTTP_ERRC_HPP
