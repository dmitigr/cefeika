// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or wsbe.hpp

#ifndef DMITIGR_WSBE_LISTENER_OPTIONS_HPP
#define DMITIGR_WSBE_LISTENER_OPTIONS_HPP

#include "dmitigr/wsbe/dll.hpp"
#include "dmitigr/wsbe/types_fwd.hpp"

#include <dmitigr/util/types_fwd.hpp>

#include <memory>
#include <string>

namespace dmitigr::wsbe {

/**
 * @brief WebSockets Listener options.
 */
class Listener_options {
public:
  /**
   * @brief The destructor.
   */
  virtual DMITIGR_WSBE_API ~Listener_options();

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
  DMITIGR_WSBE_API Listener_options(std::string address, int port);

  /**
   * @brief Copy-constructible.
   */
  DMITIGR_WSBE_API Listener_options(const Listener_options& rhs);

  /**
   * @brief Copy-assignable.
   */
  DMITIGR_WSBE_API Listener_options& operator=(const Listener_options& rhs);

  /**
   * @brief Move-constructible.
   */
  Listener_options(Listener_options&& rhs) = default;

  /**
   * @brief Move-assignable.
   */
  Listener_options& operator=(Listener_options&& rhs) = default;

  /// @}

  /**
   * @returns The endpoint identifier.
   */
  DMITIGR_WSBE_API const net::Endpoint_id* endpoint_id() const;

private:
  std::unique_ptr<detail::iListener_options> rep_;
};

} // namespace dmitigr::wsbe

#ifdef DMITIGR_WSBE_HEADER_ONLY
#include "dmitigr/wsbe/listener_options.cpp"
#endif

#endif  // DMITIGR_WSBE_LISTENER_OPTIONS_HPP
