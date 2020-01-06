// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#ifndef DMITIGR_WS_LISTENER_HPP
#define DMITIGR_WS_LISTENER_HPP

#include "dmitigr/ws/basics.hpp"
#include "dmitigr/ws/dll.hpp"
#include "dmitigr/ws/types_fwd.hpp"

#include <memory>

namespace dmitigr::ws {

/**
 * @brief A WebSockets listener.
 */
class Listener {
public:
  /// @brief Alias of Listener_options.
  using Options = Listener_options;

  /**
   * @brief The destructor.
   */
  virtual DMITIGR_WS_API ~Listener();

  /**
   * @brief The constructor.
   */
  DMITIGR_WS_API Listener();

  /// @name Constructors
  /// @{

  /**
   * @brief Constructs an instance of Listener.
   */
  explicit DMITIGR_WS_API Listener(const Options& options);

  /// @}

  /**
   * @returns Options of the listener.
   */
  virtual DMITIGR_WS_API const Options& options() const;

  /**
   * @returns `true` if the listener is listening for new client connections,
   * or `false` otherwise.
   */
  DMITIGR_WS_API bool is_listening() const;

  /**
   * @brief Starts listening.
   *
   * @par Requires
   * `!is_listening()`.
   */
  DMITIGR_WS_API void listen();

  /**
   * @brief Stops listening.
   */
  DMITIGR_WS_API void close();

private:
  /**
   * @returns The new connection instance, or `nullptr` to reject the connection.
   *
   * @brief This function to be called on every accepted connection
   * in order to create an instance of class derived from Connection.
   */
  virtual std::shared_ptr<Connection> make_connection(const Http_request* handshake) const = 0;

private:
  friend detail::iListener;
  template<bool> friend class detail::Lstnr;

  std::unique_ptr<detail::iListener> rep_;
};

} // namespace dmitigr::ws

#ifdef DMITIGR_WS_HEADER_ONLY
#include "dmitigr/ws/listener.cpp"
#endif

#endif  // DMITIGR_WS_LISTENER_HPP
