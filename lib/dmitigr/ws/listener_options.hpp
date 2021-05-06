// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#ifndef DMITIGR_WS_LISTENER_OPTIONS_HPP
#define DMITIGR_WS_LISTENER_OPTIONS_HPP

#include "dll.hpp"
#include "types_fwd.hpp"

#include "../misc/filesystem.hpp"
#include "../net/types_fwd.hpp"

#include <chrono>
#include <memory>
#include <optional>
#include <string>

namespace dmitigr::ws {

/// A WebSocket Listener options.
class Listener_options {
public:
  /// The destructor.
  virtual DMITIGR_WS_API ~Listener_options();

  /// @name Constructors
  /// @{

  /**
   * @brief Constructs a new instance of the options for listeners of network.
   *
   * @param address - IPv4 or IPv6 address to use for binding on.
   * @param port - the port number to use for binding on.
   *
   * @par Requires
   * `address` is a valid IP address and `(port > 0)`.
   */
  DMITIGR_WS_API Listener_options(std::string address, int port);

  /// Copy-constructible.
  DMITIGR_WS_API Listener_options(const Listener_options& rhs);

  /// Copy-assignable.
  DMITIGR_WS_API Listener_options& operator=(const Listener_options& rhs);

  /// Move-constructible.
  DMITIGR_WS_API Listener_options(Listener_options&& rhs);

  /// Move-assignable.
  DMITIGR_WS_API Listener_options& operator=(Listener_options&& rhs);

  /**
   * @returns A copy of this instance.
   *
   * @remarks This is used by Listener upon construction.
   */
  virtual DMITIGR_WS_API std::unique_ptr<Listener_options> to_listener_options() const;

  /// @}

  /// @returns The endpoint identifier.
  DMITIGR_WS_API const net::Endpoint& endpoint() const;

  /// Enables the HTTP functionality.
  DMITIGR_WS_API Listener_options& set_http_enabled(bool value);

  /// @returns `true` if the HTTP functionality is enabled.
  DMITIGR_WS_API bool is_http_enabled() const;

  /**
   * @brief Sets the timeout of the idle connections.
   *
   * @param value - the value of `std::nullopt` means *eternity*.
   *
   * @remarks Actually, the `value` will be rounded to seconds in the current
   * implementation.
   *
   * @see idle_timeout().
   */
  virtual DMITIGR_WS_API Listener_options& set_idle_timeout(std::optional<std::chrono::milliseconds> value);

  /**
   * @return The current value of the idle timeout.
   *
   * @see set_idle_timeout().
   */
  DMITIGR_WS_API std::optional<std::chrono::milliseconds> idle_timeout() const;

  /**
   * @brief Sets the maximum payload data size.
   *
   * @par Requires
   * `(value <= std::numeric_limits<int>::max())`
   *
   * @see max_payload_size().
   */
  virtual DMITIGR_WS_API Listener_options& set_max_payload_size(std::size_t value);

  /**
   * @return The current value of the maximum size of received message.
   *
   * @see set_max_payload_size().
   */
  DMITIGR_WS_API std::size_t max_payload_size() const;

  /**
   * @brief Sets the maximum buffered data amount.
   *
   * @par Requires
   * `(value <= std::numeric_limits<int>::max())`
   *
   * @see max_buffered_amount().
   */
  virtual DMITIGR_WS_API Listener_options& set_max_buffered_amount(std::size_t value);

  /**
   * @return The current value of the maximum buffered data amount.
   *
   * @see set_max_buffered_amount(), Connection::buffered_amount().
   */
  DMITIGR_WS_API std::size_t max_buffered_amount() const;

  /// @name SSL options
  /// @{

  /**
   * @brief Sets the SSL mode enabled if `(value == true)`.
   *
   * @par Requires
   * The library must be compiled with DMITIGR_CEFEIKA_OPENSSL.
   *
   * @see is_ssl_enabled().
   */
  DMITIGR_WS_API Listener_options& set_ssl_enabled(bool value);

  /**
   * @returns The current value of the option.
   *
   * @see set_ssl_enabled().
   */
  DMITIGR_WS_API bool is_ssl_enabled() const;

  /**
   * @brief Sets the password for encrypted PEM file.
   *
   * @par Requires
   * `(!value || !value->empty())`.
   *
   * @see ssl_pem_file_password().
   */
  DMITIGR_WS_API Listener_options& set_ssl_pem_file_password(std::optional<std::string> value);

  /**
   * @returns The current value of the option.
   *
   * @see set_ssl_pem_file_password().
   */
  DMITIGR_WS_API const std::optional<std::string>& ssl_pem_file_password() const;

  /**
   * @brief Sets the name of the file containing a SSL client certificate.
   *
   * @par Requires
   * `(is_ssl_enabled() && (!value || !value->empty()))`.
   *
   * @see ssl_certificate_file().
   */
  DMITIGR_WS_API Listener_options& set_ssl_certificate_file(std::optional<std::filesystem::path> value);

  /**
   * @returns The current value of the option.
   *
   * @see set_ssl_certificate_file().
   */
  DMITIGR_WS_API const std::optional<std::filesystem::path>& ssl_certificate_file() const;

  /**
   * @brief Sets the name of the file containing a SSL client private key.
   *
   * @par Requires
   * `(is_ssl_enabled() && (!value || !value->empty()))`.
   *
   * @see ssl_private_key_file().
   */
  DMITIGR_WS_API Listener_options& set_ssl_private_key_file(std::optional<std::filesystem::path> value);

  /**
   * @returns The current value of the option.
   *
   * @see set_ssl_private_key_file().
   */
  DMITIGR_WS_API const std::optional<std::filesystem::path>& ssl_private_key_file() const;

  /**
   * @brief Sets the name of the file containing a SSL client certificate
   * authority (CA).
   *
   * If this option is set, a verification that the WebSocket server
   * certificate is issued by a trusted certificate authority (CA) will
   * be performed.
   *
   * @par Requires
   * `(is_ssl_enabled() && (!value || !value->empty()))`.
   *
   * @see ssl_certificate_authority_file().
   */
  DMITIGR_WS_API Listener_options& set_ssl_certificate_authority_file(std::optional<std::filesystem::path> value);

  /**
   * @returns The current value of the option.
   *
   * @see set_ssl_certificate_authority_file().
   */
  DMITIGR_WS_API const std::optional<std::filesystem::path>& ssl_certificate_authority_file() const;

  /**
   * @brief Sets the name of the file containing Diffie-Hellman parameters.
   *
   * @par Requires
   * `(is_ssl_enabled() && (!value || !value->empty()))`.
   *
   * @see ssl_dh_parameters_file().
   */
  DMITIGR_WS_API Listener_options& set_ssl_dh_parameters_file(std::optional<std::filesystem::path> value);

  /**
   * @returns The current value of the option.
   *
   * @see set_ssl_dh_parameters_file().
   */
  DMITIGR_WS_API const std::optional<std::filesystem::path>& ssl_dh_parameters_file() const;

  /// @}

  /// Swaps `*this` with `other`.
  DMITIGR_WS_API void swap(Listener_options& other);

private:
  std::unique_ptr<detail::iListener_options> rep_;
};

} // namespace dmitigr::ws

#ifdef DMITIGR_WS_HEADER_ONLY
#include "listener_options.cpp"
#endif

#endif  // DMITIGR_WS_LISTENER_OPTIONS_HPP
