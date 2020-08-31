// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#include "dmitigr/pgfe/std_system_error.hpp"
#include "dmitigr/pgfe/problem.hpp"

namespace dmitigr::pgfe {

DMITIGR_PGFE_INLINE const char* Client_error_category::name() const noexcept
{
  return "dmitigr_pgfe_client_error";
}

DMITIGR_PGFE_INLINE std::string Client_error_category::message(const int ev) const
{
  std::string result(name());
  result += ' ';
  result += std::to_string(ev);
  if (const char* const literal = to_literal(static_cast<Client_errc>(ev))) {
    result += ' ';
    result += literal;
  }
  return result;
}

DMITIGR_PGFE_INLINE const char* Server_error_category::name() const noexcept
{
  return "dmitigr_pgfe_server_error";
}

DMITIGR_PGFE_INLINE std::string Server_error_category::message(const int ev) const
{
  std::string result(name());
  result += ' ';
  result += std::to_string(ev);
  result += ' ';
  result += Problem::sqlstate_int_to_string(ev);
  if (const char* const literal = to_literal(static_cast<Server_errc>(ev))) {
    result += ' ';
    result += literal;
  }
  return result;
}

// =============================================================================

DMITIGR_PGFE_INLINE const Client_error_category& client_error_category() noexcept
{
  static const Client_error_category result;
  return result;
}

DMITIGR_PGFE_INLINE const Server_error_category& server_error_category() noexcept
{
  static const Server_error_category result;
  return result;
}

DMITIGR_PGFE_INLINE std::error_code make_error_code(Client_errc errc) noexcept
{
  return std::error_code{static_cast<int>(errc), client_error_category()};
}

DMITIGR_PGFE_INLINE std::error_code make_error_code(Server_errc errc) noexcept
{
  return std::error_code{static_cast<int>(errc), server_error_category()};
}

DMITIGR_PGFE_INLINE std::error_condition make_error_condition(Client_errc errc) noexcept
{
  return std::error_condition{static_cast<int>(errc), client_error_category()};
}

DMITIGR_PGFE_INLINE std::error_condition make_error_condition(Server_errc errc) noexcept
{
  return std::error_condition{static_cast<int>(errc), server_error_category()};
}

} // namespace dmitigr::pgfe
