// -*- C++ -*-
// Copyright (C) 2020 Dmitry Igrishin
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

#include "dmitigr/wscl/version.hpp"

#include <uwsc.h>

#include <cassert>
#include <cstring>
#include <chrono>
#include <memory>
#include <stdexcept>
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
  Connection_options& url(std::string value)
  {
    url_ = std::move(value);
    return *this;
  }

  /// @returns The current WebSocket server URL.
  const std::string& url() const noexcept
  {
    return url_;
  }

  /**
   * @brief Sets the ping interval.
   *
   * @remarks `value` less then `0` disables pings.
   */
  Connection_options& ping_interval(const std::chrono::seconds value)
  {
    ping_interval_ = value;
    return *this;
  }

  /// @returns The current ping interval.
  std::chrono::seconds ping_interval() const noexcept
  {
    return ping_interval_;
  }

  /// Adds the extra header to pass upon handshake.
  Connection_options& extra_header(const std::string_view name, const std::string_view value)
  {
    extra_headers_.append(name).append(":").append(value).append("\r\n");
    return *this;
  }

  /// @returns The extra headers to pass upon handshake.
  const std::string& extra_headers() const noexcept
  {
    return extra_headers_;
  }

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
  ~Connection()
  {
    close(UWSC_CLOSE_STATUS_NORMAL);
  }

  /// Default-constructible.
  Connection() = default;

  /// Constructs an instance and initiates connection open.
  Connection(void* const loop, Options options)
    : options_{std::move(options)}
  {
    assert(loop);

    rep_.reset(uwsc_new(static_cast<struct ev_loop*>(loop), options_.url().c_str(),
        options_.ping_interval().count(), options_.extra_headers().c_str()));
    if (!rep_)
      throw std::runtime_error{"cannot create instance of type uwsc::Connection"};

    rep_->ext = this;
    rep_->onopen = handle_open__;
    rep_->onmessage = handle_message__;
    rep_->onerror = handle_error__;
    rep_->onclose = handle_close__;
  }

  /**
   * @returns `true` if the connection is open.
   *
   * @remarks Starts return `true` just before call of handle_open().
   *
   * @see handle_open().
   */
  bool is_open() const noexcept
  {
    return is_open_;
  }

  /// @returns Connection options.
  const Options& options() const noexcept
  {
    return options_;
  }

  /**
   * Sets the ping interval on open connection. (Overwrites
   * options().ping_interval().)
   *
   * @par Requires
   * `is_open()`.
   */
  void set_ping_interval(const std::chrono::seconds interval) noexcept
  {
    assert(is_open_);
    options_.ping_interval(interval);
    rep_->ping_interval = interval.count();
  }

  /**
   * Sends the data of specified format to the WebSocket server.
   *
   * @par Requires
   * `is_open()`.
   */
  void send(const std::string_view data, const bool is_binary)
  {
    assert(is_open_);
    rep_->send(rep_.get(), data.data(), data.size(), is_binary ? UWSC_OP_BINARY : UWSC_OP_TEXT);
  }

  /**
   * Sends the text data to the WebSocket server.
   *
   * @par Requires
   * `is_open()`.
   */
  void send_text(const std::string_view data)
  {
    assert(is_open_);
    rep_->send(rep_.get(), data.data(), data.size(), UWSC_OP_TEXT);
  }

  /**
   * Sends the binary data to the WebSocket server.
   *
   * @par Requires
   * `is_open()`.
   */
  void send_binary(const std::string_view data)
  {
    assert(is_open_);
    rep_->send(rep_.get(), data.data(), data.size(), UWSC_OP_BINARY);
  }

  /**
   * Pings the WebSocket server.
   *
   * @par Requires
   * `is_open()`.
   */
  void ping()
  {
    assert(is_open_);
    rep_->ping(rep_.get());
  }

  /**
   * Initiates connection close.
   *
   * @remarks is_open() starts return `false` only just before call of handle_close().
   */
  void close(const int code, const std::string& reason = {}) noexcept
  {
    if (is_open_)
      rep_->send_close(rep_.get(), code, reason.c_str());
  }

protected:
  /// @name Callbacks
  /// @remarks These functions are called on the thread of the associated event loop.
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

  static void handle_open__(uwsc_client* const cl) noexcept
  {
    auto* const s = self(cl);
    s->is_open_ = true;
    s->handle_open();
  }

  static void handle_message__(uwsc_client* const cl, void* const data,
    const std::size_t size, const bool binary) noexcept
  {
    self(cl)->handle_message({static_cast<char*>(data), size}, binary);
  }

  static void handle_error__(uwsc_client* const cl, const int code, const char* const message) noexcept
  {
    auto* const s = self(cl);
    s->is_open_ = false;
    s->handle_error(code, {message, std::strlen(message)});
  }

  static void handle_close__(uwsc_client* const cl, const int code, const char* const reason) noexcept
  {
    auto* const s = self(cl);
    s->is_open_ = false;
    s->handle_close(code, {reason, std::strlen(reason)});
  }

  static Connection* self(uwsc_client* const cl) noexcept
  {
    assert(cl);
    auto* const self = static_cast<Connection*>(cl->ext);
    assert(self);
    return self;
  }
};

} // namespace dmitigr::wscl

#endif  // DMITIGR_WSCL_WSCL_HPP
