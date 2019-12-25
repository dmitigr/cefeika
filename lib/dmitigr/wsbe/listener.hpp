// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or wsbe.hpp

#ifndef DMITIGR_WSBE_LISTENER_HPP
#define DMITIGR_WSBE_LISTENER_HPP

#include "dmitigr/wsbe/basics.hpp"
#include "dmitigr/wsbe/dll.hpp"
#include "dmitigr/wsbe/types_fwd.hpp"

#include <functional>
#include <memory>

namespace dmitigr::wsbe {

/**
 * @brief A WebSockets listener.
 */
class Listener {
public:
  /**
   * @brief Denotes the server connection maker.
   *
   * This function to be called on every accepted connection in order to create
   * an instance of class derived from Server_connection.
   */
  using Connection_maker = std::function<std::shared_ptr<Server_connection> ()>;

  /**
   * @brief The destructor.
   */
  virtual ~Listener() = default;

  /// @name Constructors
  /// @{

  /**
   * @returns An instance of the listener.
   */
  static DMITIGR_WSBE_API std::unique_ptr<Listener> make(const Listener_options* options);

  /// @}

  /**
   * @returns Options of the listener.
   */
  virtual const Listener_options* options() const = 0;

  /**
   * @returns `true` if the listener is listening for new client connections, or
   * `false` otherwise.
   */
  virtual bool is_listening() const = 0;

  /**
   * @brief Starts listening.
   *
   * @par Requires
   * `!is_listening()`.
   */
  virtual void listen() = 0;

  /**
   * @brief Stops listening.
   */
  virtual void close() = 0;

  /**
   * @brief Sets the connection maker.
   */
  virtual void set_connection_maker(Connection_maker maker);

  /**
   * @returns The connection maker.
   */
  virtual Connection_maker connection_maker() const = 0;

private:
  friend detail::iListener;

  Listener() = default;
};

} // namespace dmitigr::wsbe

#ifdef DMITIGR_WSBE_HEADER_ONLY
#include "dmitigr/wsbe/listener.cpp"
#endif

#endif  // DMITIGR_WSBE_LISTENER_HPP
