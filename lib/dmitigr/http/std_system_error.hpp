// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_STD_SYSTEM_ERROR_HPP
#define DMITIGR_HTTP_STD_SYSTEM_ERROR_HPP

#include "dmitigr/http/dll.hpp"
#include "dmitigr/http/errc.hpp"

#include <system_error>

namespace dmitigr::http {

/**
 * @brief A category of runtime server errors.
 *
 * @see Server_error.
 */
class Server_error_category final : public std::error_category {
public:
  /**
   * @returns The literal `dmitigr_http_server_error`.
   */
  const char* name() const noexcept override;

  /**
   * @returns The string that describes the error condition denoted by `ev`.
   *
   * @par Requires
   * `ev` must corresponds to the value of Server_errc.
   *
   * @remarks The caller should not rely on the return value as it is a
   * subject to change.
   */
  std::string message(int ev) const override;
};

/**
 * @returns The reference to the instance of type Server_error_category.
 */
DMITIGR_HTTP_API const Server_error_category& server_error_category() noexcept;

/**
 * @ingroup errors
 *
 * @returns `std::error_code(int(errc), server_error_category())`
 */
DMITIGR_HTTP_API std::error_code make_error_code(Server_errc errc) noexcept;

/**
 * @ingroup errors
 *
 * @returns `std::error_condition(int(errc), server_error_category())`
 */
DMITIGR_HTTP_API std::error_condition make_error_condition(Server_errc errc) noexcept;

} // namespace dmitigr::http

namespace std {

/**
 * @ingroup errors
 *
 * @brief The full specialization for integration with `<system_error>`.
 */
template<> struct is_error_code_enum<dmitigr::http::Server_errc> final : true_type {};

} // namespace std

#ifdef DMITIGR_HTTP_HEADER_ONLY
#include "dmitigr/http/std_system_error.cpp"
#endif

#endif  // DMITIGR_HTTP_STD_SYSTEM_ERROR_HPP