// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#ifndef DMITIGR_WS_LISTENER_HPP
#define DMITIGR_WS_LISTENER_HPP

#include "dmitigr/ws/basics.hpp"
#include "dmitigr/ws/dll.hpp"
#include "dmitigr/ws/types_fwd.hpp"

#include <functional>
#include <memory>
#include <string>
#include <string_view>

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
   * @brief Schedules the stopping of listening.
   *
   * @par Thread safety
   * Thread-safe.
   */
  DMITIGR_WS_API void close();

  /**
   * @brief Schedules the closing of all opened WebSocket connections.
   *
   * @par Thread safety
   * Thread-safe.
   */
  DMITIGR_WS_API void close_connections(int code, std::string reason);

  /// @name Event-loop
  /// @{

  /**
   * @brief Schedules the `callback` to be called on the thread of the event
   * loop associated with this listener.
   *
   * @par Thread safety
   * Thread-safe.
   */
  DMITIGR_WS_API void event_loop_call_soon(std::function<void()> callback);

  /**
   * @brief Schedules the `callback` to be called on the thread of the event
   * loop associated with this listener.
   *
   * @par Thread safety
   * Thread-safe.
   */
  DMITIGR_WS_API void for_each(std::function<void(Connection*)> callback);

  /// @}

  /// @name Timers
  /// @{

  /**
   * @returns The number of timers.
   */
  DMITIGR_WS_API std::size_t timer_count() const;

  /**
   * @returns The timer index if it has been added, or `std::nullopt` otherwise.
   */
  DMITIGR_WS_API std::optional<std::size_t> timer_index(std::string_view name) const;

  /**
   * @brief Creates the timer associated with the specified `name`.
   *
   * @returns The reference to the created timer.
   */
  DMITIGR_WS_API Timer& add_timer(std::string name);

  /**
   * @brief Removes the timer associated with the specified `name`, or do
   * nothing if no such a timer presents.
   */
  DMITIGR_WS_API void remove_timer(std::string_view name);

  /**
   * @returns The timer associated with the specified `name`, or `nullptr` if
   * no such a timer presents.
   */
  DMITIGR_WS_API Timer* timer(std::string_view name) const;

  /**
   * @returns The timer by position.
   *
   * @par Requires
   * `(pos < timer_count())`.
   *
   * @throws std::out_of_range.
   */
  DMITIGR_WS_API Timer& timer(std::size_t pos) const;

  /**
   * @returns The name of timer at position `pos`.
   *
   * @par Requires
   * `(pos < timer_count())`.
   *
   * @throws std::out_of_range.
   */
  DMITIGR_WS_API const std::string& timer_name(std::size_t pos) const;

  /// @}

private:
  /**
   * @brief This function is called on every opening handshake (HTTP Upgrade
   * request) from a WebSocket client.
   *
   * Handshake from the server may be either implicitly rejected (HTTP response
   * with status code of `500` will be send to the client) or implicitly completed
   * by just returning `nullptr` or a new Connection instance accordingly.
   *
   * Handshake from the server may be deferred by setting abort handler on `io`
   * and returning `nullptr` or a new Connection instance. Deferred handshake
   * can be either explicitly rejected or explicitly completed later by sending
   * a custom HTTP response or by calling Http_io::end_handshake() method
   * accordingly.
   *
   * @param req A HTTP request.
   * @param io An IO object which should be touched only in cases of explicit
   * rejections or completions (either deferred or not). To defer the handshake
   * the abort handler must be set on this object.
   *
   * @par Postconditions
   * For any handshake completion:
   * `io->is_valid()`.
   *
   * For implicit handshake completion:
   * `!io->is_response_handler_set()`.
   *
   * @returns The result may be:
   *   - `nullptr` to reject the handshake implicitly or to denote the explicit
   *   (either deferred or not) rejection of a handshake;
   *   - a new Connection instance to complete handshake implicitly or to defer
   *   the handshake (for either rejection or completion).
   *
   * @remarks The behaviour is undefined if `io` has been used for sending any
   * data from within this function in cases of implicit rejection or completion
   * of the handshake.
   *
   * @see Http_io.
   */
  virtual std::shared_ptr<Connection> handle_handshake(const Http_request& req,
    std::shared_ptr<Http_io> io) = 0;

  /**
   * @brief This function to be called on every HTTP request if HTTP
   * functionality is enabled in Listener_options.
   *
   * @param req A HTTP request.
   * @param io An IO object To defer the handshake the abort handler must be set
   * on this object.
   *
   * @remarks `io` will be aborted in case if it's unfinished and it doesn't have
   * the abort handler set immediately after returning from this function.
   *
   * @see Listener_options.
   */
  virtual void handle_request(const Http_request& req, std::shared_ptr<Http_io> io);

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
