// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#ifndef DMITIGR_WS_HTTP_IO_HPP
#define DMITIGR_WS_HTTP_IO_HPP

#include "dmitigr/ws/types_fwd.hpp"

#include <functional>
#include <string_view>
#include <utility>

namespace dmitigr::ws {

/**
 * @brief A HTTP I/O.
 */
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
   * Parameters:
   *   - `data` - portion of incoming data to consume
   *   - `is_last` - flag which indicates whether the `data` is the last portion
   *   of the incoming data or not.
   *
   * @see set_request_handler().
   */
  using Request_handler = std::function<void(std::string_view data, bool is_last)>;

  /*
   * @brief An alias of a function to handle sending of a response.
   *
   * The parameter `position` denotes a starting position of data to be used
   * for a next call of send_response().
   *
   * The function must returns `true` if send operation was successful, or
   * `false` otherwise.
   *
   * @see respond().
   */
  using Response_handler = std::function<bool(int position)>;

  /**
   * @brief The destructor.
   */
  virtual ~Http_io() = default;

  /**
   * @returns `true` if calling of any method other than the destructor or
   * is_valid() will not lead to the undefined behavior, or `false` otherwise.
   */
  virtual bool is_valid() const = 0;

  /**
   * @brief Sends the Status-Line to the remote side.
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
   * @brief Initiates the sending of a response.
   *
   * @param data A data to send
   * @param handler A handler for responding in a piecewise fashion. If
   * no handler specified then the entire data will be send upon a call.
   *
   * @returns `true` if all the data was sent upon this call, or `false`
   * otherwise.
   *
   * @par Requires
   * `(is_valid() && !is_response_handler_set())`.
   *
   * @par Effects
   * If `handler` is not specified, then (is_valid() == false) after calling
   * this method. If `handler` is specified, then (is_valid() == false) after
   * the specified `handler` returns `true`.
   *
   * @remarks For a large data handler should be used.
   */
  virtual bool respond(std::string_view data, Response_handler handler = {}) = 0;

  /**
   * @brief Attempts to send a portion of the response.
   *
   * @param data A data part to send
   * @param total_size A total size of data to send
   *
   * @returns A pair of two booleans:
   *   -# indicates success of send operation. If `false` then there is no
   *   readiness to send more data at the moment.
   *   -# indicates whether the `total_size` of bytes were sent in summary
   *   after this call. If `false` then the handler which was set by respond()
   *   method will be called at the moment of readiness to send more data.
   *
   * @par Requires
   * `(is_valid() && is_response_handler_set() &&
   *   (total_size >= 0) && (data.size() <= total_size))`.
   *
   * @remarks This method should called from a respond handler. The respond
   * handler should return a first boolean of the value returned by this
   * method.
   */
  virtual std::pair<bool, bool> send_response(std::string_view data, int total_size) = 0;

  /**
   * @returns `true` if the respond handler was set, or `false` otherwise.
   *
   * @see respond(std::string_view, Response_handler).
   */
  virtual bool is_response_handler_set() const = 0;

  /**
   * @brief Closes the communication immediately (abnormally).
   *
   * @par Requires
   * `is_valid()`.
   *
   * @see set_abort_handler().
   */
  virtual void abort() = 0;

  /**
   * @brief Sets the abort handler.
   *
   * @par Requires
   * `(is_valid() && !is_abort_handler_set())`.
   *
   * @see Abort_handler.
   */
  virtual void set_abort_handler(Abort_handler handler) = 0;

  /**
   * @returns `true` if the abort handler was set, or `false` otherwise.
   *
   * @see Abort_handler.
   */
  virtual bool is_abort_handler_set() const = 0;

  /**
   * @brief Sets the request handler.
   *
   * @par Requires
   * `(is_valid() && !is_request_handler_set())`.
   *
   * @see Request_handler.
   */
  virtual void set_request_handler(Request_handler handler) = 0;

  /**
   * @returns `true` if the request handler was set, or `false` otherwise.
   *
   * @see Request_handler.
   */
  virtual bool is_request_handler_set() const = 0;

private:
  friend detail::iHttp_io;

  Http_io() = default;
};

} // namespace dmitigr::ws

#ifdef DMITIGR_WS_HEADER_ONLY
#include "dmitigr/ws/http_io.cpp"
#endif

#endif  // DMITIGR_WS_HTTP_IO_HPP
