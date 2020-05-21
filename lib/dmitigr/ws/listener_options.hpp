// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#ifndef DMITIGR_WS_LISTENER_OPTIONS_HPP
#define DMITIGR_WS_LISTENER_OPTIONS_HPP

#include "dmitigr/ws/dll.hpp"
#include "dmitigr/ws/types_fwd.hpp"

#include <dmitigr/net/types_fwd.hpp>

#include <chrono>
#include <memory>
#include <optional>
#include <string>

namespace dmitigr::ws {

/**
 * @brief WebSockets Listener options.
 */
class Listener_options {
public:
  /**
   * @brief The destructor.
   */
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

  /**
   * @brief Copy-constructible.
   */
  DMITIGR_WS_API Listener_options(const Listener_options& rhs);

  /**
   * @brief Copy-assignable.
   */
  DMITIGR_WS_API Listener_options& operator=(const Listener_options& rhs);

  /**
   * @brief Move-constructible.
   */
  DMITIGR_WS_API Listener_options(Listener_options&& rhs);

  /**
   * @brief Move-assignable.
   */
  DMITIGR_WS_API Listener_options& operator=(Listener_options&& rhs);

  /**
   * @returns A copy of this instance.
   */
  virtual DMITIGR_WS_API std::unique_ptr<Listener_options> to_listener_options() const;

  /// @}

  /**
   * @returns The endpoint identifier.
   */
  DMITIGR_WS_API const net::Endpoint& endpoint() const;

  /**
   * @brief Enables the HTTP functionality.
   */
  DMITIGR_WS_API Listener_options& set_http_enabled(bool value);

  /**
   * @returns `true` if the HTTP functionality is enabled, or `false` otherwise.
   */
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
   * @return The current value of the maximum payload data size.
   *
   * @see set_max_payload_size().
   */
  DMITIGR_WS_API std::size_t max_payload_size() const;

  /**
   * @brief Swaps `*this` with `other`.
   */
  DMITIGR_WS_API void swap(Listener_options& other);

private:
  std::unique_ptr<detail::iListener_options> rep_;
};

} // namespace dmitigr::ws

#ifdef DMITIGR_WS_HEADER_ONLY
#include "dmitigr/ws/listener_options.cpp"
#endif

#endif  // DMITIGR_WS_LISTENER_OPTIONS_HPP
