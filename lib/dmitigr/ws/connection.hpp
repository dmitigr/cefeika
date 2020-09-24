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

/// A WebSocket connection.
class Connection : public std::enable_shared_from_this<Connection> {
public:
  /// The destructor.
  virtual DMITIGR_WS_API ~Connection();

  /// The default-constructible.
  DMITIGR_WS_API Connection();

  /// @name Event-loop related API
  /// These functions can only be used for implementing a `callback` of
  /// event_loop_call_soon().
  /// @{

  /**
   * @brief Attempts to schedule the `callback` to the called on the thread of
   * the event loop associated with this connection.
   *
   * @returns `true` on success.
   *
   * @par Thread safety
   * Thread-safe.
   */
  DMITIGR_WS_API bool event_loop_call_soon(std::function<void()> callback);

  /**
   * @returns `true` if the underlying socket is valid.
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
   * transmitted to the remote side over the connection, possibly increasing
   * the value returned by `buffered_amount()` by the number of bytes needed
   * to contain the `data`.
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

  /// Shortcut of `send(data, Data_format::text)`.
  DMITIGR_WS_API void send_text(std::string_view data);

  /// Shortcut of `send(data, Data_format::binary)`.
  DMITIGR_WS_API void send_binary(std::string_view data);

  /**
   * @brief Closes the connection in a normal way.
   *
   * @remarks The behaviour is undefined if called not on the thread of the
   * associated event loop!
   * Has no effect if `!is_connected()`.
   *
   * @see event_loop_call_soon().
   */
  DMITIGR_WS_API void close(int code, std::string_view reason);

  /**
   * @brief Closes the connection immediately (abnormally).
   *
   * @remarks The behaviour is undefined if called not on the thread of the
   * associated event loop!
   * Has no effect if `!is_connected()`.
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

  /**
   * @brief This function is called when the amount of buffered data has been
   * changed (most likely reduced) and these changes are needed to be handled.
   *
   * Typically, this handler is called in case of back pressure, i.e. when the
   * size of the buffered data to send exceeds the set limit. Before sending
   * more data, the value returned by buffered_amount() must be taken into
   * account.
   *
   * @remarks This function is called on the thread of the associated event loop.
   *
   * @see buffered_amount(), Listener_options::set_max_buffered_amount().
   */
  virtual void handle_drain() = 0;

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
