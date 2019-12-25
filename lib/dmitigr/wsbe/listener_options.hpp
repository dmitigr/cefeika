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
  virtual ~Listener_options()  = default;

  /// @name Constructors
  /// @{

  /**
   * @returns A new instance of the options for listeners of network.
   *
   * @param address - IPv4 or IPv6 address to use for binding on.
   * @param port - the port number to use for binding on.
   *
   * @par Requires
   * `(port > 0)`.
   */
  static DMITIGR_WSBE_API std::unique_ptr<Listener_options> make(std::string address, int port);

  /**
   * @returns A new instance of the Listener initialized with this instance.
   *
   * @see Listener::make().
   */
  virtual std::unique_ptr<Listener> make_listener() const = 0;

  /**
   * @returns The copy of this instance.
   */
  virtual std::unique_ptr<Listener_options> to_listener_options() const = 0;

  /// @}

  /**
   * @returns The endpoint identifier.
   */
  virtual const net::Endpoint_id* endpoint_id() const = 0;

private:
  friend detail::iListener_options;

  Listener_options() = default;
};

} // namespace dmitigr::wsbe

#ifdef DMITIGR_WSBE_HEADER_ONLY
#include "dmitigr/wsbe/listener_options.cpp"
#endif

#endif  // DMITIGR_WSBE_LISTENER_OPTIONS_HPP
