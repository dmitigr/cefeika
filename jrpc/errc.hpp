// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#ifndef DMITIGR_JRPC_ERRC_HPP
#define DMITIGR_JRPC_ERRC_HPP

namespace dmitigr::jrpc {

/// JSON-RPC server-error codes.
enum class Server_errc {
  /// Invalid JSON was received by the server.
  parse_error = -32700,

  /// The JSON sent is not a valid Request object.
  invalid_request = -32600,

  /// The method does not exist / is not available.
  method_not_found = -32601,

  /// Invalid method parameter(s).
  invalid_params = -32602,

  /// Internal JSON-RPC error.
  internal_error = -32603,

  /// Generic error.
  generic_error = -32000
};

/**
 * @returns The literal representation of the `value`, or `nullptr` if
 * `value` does not corresponds to any value defined by Server_errc.
 */
constexpr const char* to_literal(const Server_errc value)
{
  switch (value) {
  case Server_errc::parse_error:
    return "parse_error";
  case Server_errc::invalid_request:
    return "invalid_request";
  case Server_errc::method_not_found:
    return "method_not_found";
  case Server_errc::invalid_params:
    return "invalid_params";
  case Server_errc::internal_error:
    return "internal_error";
  case Server_errc::generic_error:
    return "generic_error";
  }
  return nullptr;
}

} // namespace dmitigr::jrpc

#endif  // DMITIGR_JRPC_ERRC_HPP
