// -*- C++ -*-
// Copyright (C) 2021 Dmitry Igrishin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
// Dmitry Igrishin
// dmitigr@gmail.com

#ifndef DMITIGR_WSCL_WSCL_HPP
#define DMITIGR_WSCL_WSCL_HPP

#include "dll.hpp"
#include "version.hpp"
#include "../3rdparty/uwsc/uwsc.h"

#include <chrono>
#include <memory>
#include <string>
#include <string_view>

namespace std {

/// The default deleter for uwsc_client.
template<> struct default_delete<uwsc_client> final {
  void operator()(uwsc_client* const ptr) const
  {
    ptr->free(ptr);
  }
};

} // namespace std

namespace dmitigr::wscl {

/**
 * WebSocket connection options.
 *
 * @see Connection.
 */
class Connection_options final {
public:
  /**
   * @brief Sets the URL of WebSocket server to connect.
   *
   * URL scheme must be "ws" or "wss".
   */
  DMITIGR_WSCL_API Connection_options& url(std::string value);

  /// @returns The current WebSocket server URL.
  DMITIGR_WSCL_API const std::string& url() const noexcept;

  /**
   * @brief Sets the ping interval.
   *
   * @remarks `value` less then `0` disables pings.
   */
  DMITIGR_WSCL_API Connection_options& ping_interval(const std::chrono::seconds value);

  /// @returns The current ping interval.
  DMITIGR_WSCL_API std::chrono::seconds ping_interval() const noexcept;

  /// Adds the extra header to pass upon handshake.
  DMITIGR_WSCL_API Connection_options& extra_header(const std::string_view name,
    const std::string_view value);

  /// @returns The extra headers to pass upon handshake.
  DMITIGR_WSCL_API const std::string& extra_headers() const noexcept;

private:
  std::string url_;
  std::chrono::seconds ping_interval_;
  std::string extra_headers_;
};

/**
 * WebSocket connection.
 *
 * @see Connection_options.
 */
class Connection {
public:
  /// An alias of Connection_options.
  using Options = Connection_options;

  /// Closes connection with normal status.
  DMITIGR_WSCL_API ~Connection();

  /// Default-constructible.
  Connection() = default;

  /**
   * Constructs an instance and initiates connection open.
   *
   * @par Requires
   * `loop`.
   */
  DMITIGR_WSCL_API Connection(void* const loop, Options options);

  /**
   * @returns `true` if the connection is open.
   *
   * @remarks Starts return `true` just before call of handle_open().
   *
   * @see handle_open().
   */
  DMITIGR_WSCL_API bool is_open() const noexcept;

  /// @returns Connection options.
  DMITIGR_WSCL_API const Options& options() const noexcept;

  /**
   * Sets the ping interval on open connection. (Overwrites
   * options().ping_interval().)
   *
   * @par Requires
   * `is_open()`.
   */
  DMITIGR_WSCL_API void set_ping_interval(const std::chrono::seconds interval);

  /**
   * Sends the data of specified format to the WebSocket server.
   *
   * @par Requires
   * `is_open()`.
   */
  DMITIGR_WSCL_API void send(const std::string_view data, const bool is_binary);

  /**
   * Sends the text data to the WebSocket server.
   *
   * @par Requires
   * `is_open()`.
   */
  DMITIGR_WSCL_API void send_text(const std::string_view data);

  /**
   * Sends the binary data to the WebSocket server.
   *
   * @par Requires
   * `is_open()`.
   */
  DMITIGR_WSCL_API void send_binary(const std::string_view data);

  /**
   * Pings the WebSocket server.
   *
   * @par Requires
   * `is_open()`.
   */
  DMITIGR_WSCL_API void ping();

  /**
   * Initiates connection close.
   *
   * @remarks is_open() starts return `false` only just before call of
   * handle_close().
   */
  DMITIGR_WSCL_API void close(const int code, const std::string& reason = {}) noexcept;

protected:
  /// @name Callbacks
  /// @remarks These functions are called on the thread of the associated event
  /// loop.
  /// @{

  /// This function is called just after successful connection open.
  virtual void handle_open() = 0;

  /// This function is called just after message is received.
  virtual void handle_message(std::string_view data, bool is_binary) = 0;

  /// This function is called on error.
  virtual void handle_error(int code, std::string_view message) = 0;

  /// This function is called when the underlying socket is about to be close.
  virtual void handle_close(int code, std::string_view reason) = 0;

  /// @}

private:
  bool is_open_{};
  std::unique_ptr<uwsc_client> rep_;
  Options options_;

  static void handle_open__(uwsc_client* const cl) noexcept;

  static void handle_message__(uwsc_client* const cl, void* const data,
    const std::size_t size, const bool binary) noexcept;

  static void handle_error__(uwsc_client* const cl, const int code,
    const char* const message) noexcept;

  static void handle_close__(uwsc_client* const cl, const int code,
    const char* const reason) noexcept;

  static Connection* self(uwsc_client* const cl) noexcept;
};

} // namespace dmitigr::wscl

#ifdef DMITIGR_WSCL_HEADER_ONLY
#include "wscl.cpp"
#endif

#endif  // DMITIGR_WSCL_WSCL_HPP
