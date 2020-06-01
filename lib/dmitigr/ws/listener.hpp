// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#ifndef DMITIGR_WS_LISTENER_HPP
#define DMITIGR_WS_LISTENER_HPP

#include "dmitigr/ws/basics.hpp"
#include "dmitigr/ws/dll.hpp"
#include "dmitigr/ws/types_fwd.hpp"

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
   * @brief Stops listening.
   */
  DMITIGR_WS_API void close();

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
   * @returns The timer index if it has been added.
   *
   * @throws std::out_of_range.
   */
  DMITIGR_WS_API std::size_t timer_index_throw(std::string_view name) const;

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
   * @brief This function to be called on every accepted connection
   * in order to create an instance of class derived from Connection.
   *
   * @returns The new connection instance, or `nullptr` to reject the connection.
   */
  virtual std::shared_ptr<Connection> make_connection(const Http_request* handshake) = 0;

  /**
   * @brief This function to be called on every HTTP request if HTTP
   * functionality is enabled in Listener_options.
   *
   * @see Listener_options.
   */
  virtual void handle_request(const ws::Http_request* req,
    std::shared_ptr<ws::Http_io> res) const;

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
