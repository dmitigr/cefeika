// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#ifndef DMITIGR_JRPC_ERRC_HPP
#define DMITIGR_JRPC_ERRC_HPP

#include "dmitigr/jrpc/dll.hpp"

namespace dmitigr::jrpc {

/**
 * @brief JSON-RPC server-error codes.
 */
enum class Server_errc {
  /** Invalid JSON was received by the server. */
  parse_error = -32700,

  /** The JSON sent is not a valid Request object. */
  invalid_request = -32600,

  /** The method does not exist / is not available. */
  method_not_found = -32601,

  /** Invalid method parameter(s). */
  invalid_params = -32602,

  /** Internal JSON-RPC error. */
  internal_error = -32603
};

/**
 * @returns The literal representation of the `errc`, or `nullptr`
 * if `errc` does not corresponds to any value defined by Server_errc.
 */
DMITIGR_JRPC_API const char* to_literal(Server_errc errc);

} // namespace dmitigr::jrpc

#ifdef DMITIGR_JRPC_HEADER_ONLY
#include "dmitigr/jrpc/errc.cpp"
#endif

#endif  // DMITIGR_JRPC_ERRC_HPP
