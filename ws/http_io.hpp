// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#ifndef DMITIGR_WS_HTTP_IO_HPP
#define DMITIGR_WS_HTTP_IO_HPP

#include "types_fwd.hpp"

#include <cstdint>
#include <functional>
#include <string_view>
#include <utility>

namespace dmitigr::ws {

/// A HTTP I/O.
class Http_io {
public:
  /**
   * @brief An alias of a function to handle a cases when the communication
   * channel is closed abnormally.
   *
   * For example, when the client just closes the browser. Also, this function
   * is called if abort() is called.
   *
   * @see abort(), set_abort_handler().
   */
  using Abort_handler = std::function<void()>;

  /**
   * @brief An alias of a function to handle the incoming data of a request body.
   *
   * @param data Portion of incoming data to consume.
   * @param is_last Whether the `data` is the last portion of the incoming data?
   *
   * @see set_request_handler().
   */
  using Request_handler = std::function<void(std::string_view data, bool is_last)>;

  /*
   * @brief An alias of a function to handle sending of a response piecewise.
   *
   * The parameter `position` denotes a starting position of data to be used
   * for a next call of send_data().
   *
   * The function must returns `true` if send operation was successful.
   *
   * @see respond().
   */
  using Response_handler = std::function<bool(std::uintmax_t position)>;

  /// The destructor.
  virtual ~Http_io() = default;

  /**
   * @returns `true` if calling of any method other than the destructor or
   * is_valid() will not lead to the undefined behavior.
   */
  virtual bool is_valid() const = 0;

  /**
   * @returns A pointer to an instance of a not yet open WebSockets connection.
   *
   * @remarks During the call of Listener::handle_handshake() the returned value
   * is always `nullptr`.
   *
   * @see end_handshake(), Listener::handle_handshake().
   */
  virtual Connection* connection() = 0;

  /**
   * @brief Sends a WebSocket handshake and finishes the IO.
   *
   * @par Requires
   * `is_valid() && connection()`.
   *
   * @par Effects
   * `!is_valid() && !connection()`.
   *
   * @see connection(), Listener::handle_handshake().
   */
  virtual void end_handshake() = 0;

  /**
   * @brief Sends the Status-Line.
   *
   * @param code 3-digit HTTP status code, like 200
   * @param phrase A human-readable textual phrase, like "OK"
   *
   * @par Requires
   * `is_valid()`.
   */
  virtual void send_status(int code, std::string_view phrase) = 0;

  /**
   * @brief Sends a response-header field.
   *
   * @param name The name of header, like "Content-Type"
   * @param value The value of header, like "text/plain"
   *
   * @par Requires
   * `is_valid()`.
   */
  virtual void send_header(std::string_view name, std::string_view value) = 0;

  /**
   * @brief Attempts to send (a portion) of the response.
   *
   * If no handler set by set_response_handler() then the entire data will be
   * send upon a call which may not be suitable for a large data!
   *
   * @param data Data to send.
   * @param total_size Total size of data to send. `0` implies `data.size()`.
   *
   * @returns A pair of two booleans:
   *   -# indicates success of send operation. If `false` then there is no
   *   readiness to send more data at the moment.
   *   -# indicates whether the `total_size` of bytes were sent in summary
   *   after this call. If `false` then the handler which was set by
   *   set_response_handler() method will be called at the moment of readiness
   *   to send more data.
   *
   * @par Requires
   * `is_valid()`.
   *
   * @par Effects
   * If the response handler is not set, then `(is_valid() == false)` after
   * calling this method. Otherwise `(is_valid() == false)` after the response
   * handler returns `true`.
   *
   * @see set_response_handler().
   */
  virtual std::pair<bool, bool> send_data(std::string_view data, std::uintmax_t total_size = 0) = 0;

  /**
   * @brief Sends the `data` (if any) and finishes the IO.
   *
   * @par Effects
   * `(!is_valid())`.
   */
  virtual void end(std::string_view data = {}) = 0;

  /**
   * @brief Sets the response handler.
   *
   * @par Requires
   * `(is_valid() && !is_response_handler_set() && handler)`.
   *
   * @see Response_handler, send_data().
   */
  virtual void set_response_handler(Response_handler handler) = 0;

  /**
   * @returns `true` if the respond handler was set.
   *
   * @see set_response_handler().
   */
  virtual bool is_response_handler_set() const = 0;

  /**
   * @brief Closes the communication immediately (abnormally).
   *
   * @par Requires
   * `is_valid()`.
   *
   * @par Effects
   * `!is_valid()`.
   *
   * @see set_abort_handler().
   */
  virtual void abort() = 0;

  /**
   * @brief Sets the abort handler.
   *
   * @par Requires
   * `(is_valid() && !is_abort_handler_set() && handler)`.
   *
   * @see Abort_handler.
   */
  virtual void set_abort_handler(Abort_handler handler) = 0;

  /**
   * @returns `true` if the abort handler was set.
   *
   * @see set_abort_handler().
   */
  virtual bool is_abort_handler_set() const = 0;

  /**
   * @brief Sets the request handler.
   *
   * @par Requires
   * `(is_valid() && !is_request_handler_set() && handler)`.
   *
   * @see Request_handler.
   */
  virtual void set_request_handler(Request_handler handler) = 0;

  /**
   * @returns `true` if the request handler was set.
   *
   * @see set_request_handler().
   */
  virtual bool is_request_handler_set() const = 0;

private:
  friend detail::iHttp_io;

  Http_io() = default;
};

} // namespace dmitigr::ws

#ifdef DMITIGR_WS_HEADER_ONLY
#include "http_io.cpp"
#endif

#endif  // DMITIGR_WS_HTTP_IO_HPP
