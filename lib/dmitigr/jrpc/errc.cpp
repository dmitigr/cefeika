// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#include "dmitigr/jrpc/errc.hpp"

namespace dmitigr::jrpc {

DMITIGR_JRPC_INLINE const char* to_literal(const Server_errc errc)
{
  switch (errc) {
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
