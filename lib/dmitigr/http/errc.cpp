// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#include "dmitigr/http/errc.hpp"

namespace dmitigr::http {

DMITIGR_HTTP_INLINE const char* to_literal(const Server_errc errc)
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
