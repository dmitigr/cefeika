// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#ifndef DMITIGR_JRPC_STD_SYSTEM_ERROR_HPP
#define DMITIGR_JRPC_STD_SYSTEM_ERROR_HPP

#include "dmitigr/jrpc/errc.hpp"

#include <string>
#include <system_error>

namespace dmitigr::jrpc {

/**
 * @brief A category of runtime server errors.
 *
 * @see Server_error.
 */
class Server_error_category final : public std::error_category {
public:
  /**
   * @returns The literal `dmitigr_jrpc_server_error`.
   */
  const char* name() const noexcept override
  {
    return "dmitigr_jrpc_server_error";
  }

  /**
   * @returns The string that describes the error condition denoted by `ev`.
   *
   * @par Requires
   * `ev` must corresponds to the value of Server_errc.
   *
   * @remarks The caller should not rely on the return value as it is a
   * subject to change.
   */
  std::string message(const int ev) const override
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
};

/**
 * @returns The reference to the instance of type Server_error_category.
 */
inline const Server_error_category& server_error_category() noexcept
{
  static const Server_error_category result;
  return result;
}

/**
 * @ingroup errors
 *
 * @returns `std::error_code(int(errc), server_error_category())`
 */
inline std::error_code make_error_code(const Server_errc errc) noexcept
{
  return std::error_code{static_cast<int>(errc), server_error_category()};
}

/**
 * @ingroup errors
 *
 * @returns `std::error_condition(int(errc), server_error_category())`
 */
inline std::error_condition make_error_condition(const Server_errc errc) noexcept
{
  return std::error_condition{static_cast<int>(errc), server_error_category()};
}

} // namespace dmitigr::jrpc

namespace std {

/**
 * @ingroup errors
 *
 * @brief The full specialization for integration with `<system_error>`.
 */
template<> struct is_error_code_enum<dmitigr::jrpc::Server_errc> final : true_type {};

} // namespace std

#endif  // DMITIGR_JRPC_STD_SYSTEM_ERROR_HPP
