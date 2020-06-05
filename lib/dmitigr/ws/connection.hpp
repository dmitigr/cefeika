// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#ifndef DMITIGR_WS_CONNECTION_HPP
#define DMITIGR_WS_CONNECTION_HPP

#include "dmitigr/ws/dll.hpp"
#include "dmitigr/ws/types_fwd.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>

namespace dmitigr::ws {

/**
 * @brief A WebSockets connection.
 */
class Connection : public std::enable_shared_from_this<Connection> {
public:
  /**
   * @brief The destructor.
   */
  virtual DMITIGR_WS_API ~Connection();

  /**
   * @brief The constructor.
   */
  DMITIGR_WS_API Connection();

  /// @name Event-loop related API
  /// These functions can only be used for implementing a `callback` of
  /// event_loop_call_soon().
  /// @{

  /**
   * @brief Attempts to schedule the `callback` to the called on the thread of
   * the event loop associated with this connection.
   *
   * @par Thread safety
   * Thread-safe.
   */
  DMITIGR_WS_API void event_loop_call_soon(std::function<void()> callback);

  /**
   * @returns `true` if the connection is valid, or `false` otherwise.
   *
   * @remarks The behaviour is undefined if called not on the thread of the
   * associated event loop!
   *
   * @see event_loop_call_soon().
   */
  DMITIGR_WS_API bool is_connected() const;

  /**
   * @returns The textual representation of the remote IP address.
   *
   * @par Requires
   * `is_connected()`.
   *
   * @remarks The behaviour is undefined if called not on the thread of the
   * associated event loop!
   *
   * @see event_loop_call_soon().
   */
  DMITIGR_WS_API std::string remote_ip_address() const;

  /**
   * @returns The textual representation of the local IP address.
   *
   * @par Requires
   * `is_connected()`.
   *
   * @remarks The behaviour is undefined if called not on the thread of the
   * associated event loop!
   *
   * @see event_loop_call_soon().
   */
  DMITIGR_WS_API std::string local_ip_address() const;

  /**
   * @returns The number of bytes of queued data.
   *
   * @par Requires
   * `is_connected()`.
   *
   * @remarks The behaviour is undefined if called not on the thread of the
   * associated event loop!
   *
   * @see event_loop_call_soon().
   */
  DMITIGR_WS_API std::size_t buffered_amount() const;

  /**
   * @brief Enqueues the specified `data` of the specified `format` to be
   * transmitted to the remote side over the connection, increasing the value
   * returned by `buffered_amount()` by the number of bytes needed to contain
   * the `data`.
   *
   * @par Requires
   * `is_connected()`.
   *
   * @remarks The behaviour is undefined if called not on the thread of the
   * associated event loop!
   *
   * @see event_loop_call_soon().
   */
  DMITIGR_WS_API void send(std::string_view data, Data_format format);

  /**
   * @brief Closes the connection in a normal way.
   *
   * @remarks The behaviour is undefined if called not on the thread of the
   * associated event loop!
   *
   * @see event_loop_call_soon().
   */
  DMITIGR_WS_API void close(int code, std::string_view reason);

  /**
   * @brief Closes the connection immediately (abnormally).
   *
   * @remarks The behaviour is undefined if called not on the thread of the
   * associated event loop!
   *
   * @see event_loop_call_soon().
   */
  DMITIGR_WS_API void abort();

  /// @}

private:
  /**
   * @brief This function is called when a message is received from a remote side.
   *
   * @remarks This function is called on the thread of the associated event loop.
   */
  virtual void handle_message(std::string_view data, Data_format format) = 0;

  /**
   * @brief This function is called when a connection is about to be closed.
   *
   * @remarks This function is called on the thread of the associated event loop.
   * @remarks The underlying socket may be destroyed before the moment of this
   * function call. Hence, the implementation must use `is_connected()` before
   * using the API which requires `(is_connected() == true)` as a precondition.
   */
  virtual void handle_close(int code, std::string_view reason) = 0;

private:
  friend detail::iListener;
  template<bool> friend class detail::Lstnr;

  mutable std::mutex mut_;
  std::unique_ptr<detail::iConnection> rep_;
};

} // namespace dmitigr::ws

#ifdef DMITIGR_WS_HEADER_ONLY
#include "dmitigr/ws/connection.cpp"
#endif

#endif  // DMITIGR_WS_CONNECTION_HPP
