// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#include "dmitigr/jrpc/std_system_error.hpp"

namespace dmitigr::jrpc {

DMITIGR_JRPC_INLINE const char* Server_error_category::name() const noexcept
{
  return "dmitigr_jrpc_server_error";
}

DMITIGR_JRPC_INLINE std::string Server_error_category::message(const int ev) const
{
  std::string result(name());
  result += ' ';
  result += std::to_string(ev);
  result += ' ';
  if (const char* const literal = to_literal(static_cast<Server_errc>(ev))) {
    result += ' ';
    result += literal;
  }
  return result;
}

// =============================================================================

DMITIGR_JRPC_INLINE const Server_error_category& server_error_category() noexcept
{
  static const Server_error_category result;
  return result;
}

DMITIGR_JRPC_INLINE std::error_code make_error_code(const Server_errc errc) noexcept
{
  return std::error_code{static_cast<int>(errc), server_error_category()};
}

DMITIGR_JRPC_INLINE std::error_condition make_error_condition(const Server_errc errc) noexcept
{
  return std::error_condition{static_cast<int>(errc), server_error_category()};
}

} // namespace dmitigr::jrpc
