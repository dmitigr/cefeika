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

class Connection_options final {
public:
  Connection_options& url(std::string value)
  {
    url_ = std::move(value);
    return *this;
  }

  const std::string& url() const noexcept
  {
    return url_;
  }

  Connection_options& ping_interval(const std::chrono::seconds value)
  {
    ping_interval_ = value;
    return *this;
  }

  std::chrono::seconds ping_interval() const noexcept
  {
    return ping_interval_;
  }

  Connection_options& extra_header(const std::string_view name, const std::string_view value)
  {
    extra_headers_.append(name).append(":").append(value).append("\r\n");
    return *this;
  }

  const std::string& extra_headers() const noexcept
  {
    return extra_headers_;
  }

private:
  std::string url_;
  std::chrono::seconds ping_interval_;
  std::string extra_headers_;
};

class Connection {
public:
  using Options = Connection_options;

  ~Connection()
  {
    close(UWSC_CLOSE_STATUS_NORMAL);
  }

  Connection() = default;

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

  bool is_open() const noexcept
  {
    return is_open_;
  }

  const Options& options() const noexcept
  {
    return options_;
  }

  void set_ping_interval(const std::chrono::seconds interval) noexcept
  {
    options_.ping_interval(interval);
    rep_->ping_interval = interval.count();
  }

  std::chrono::seconds ping_interval() const noexcept
  {
    return std::chrono::seconds{rep_->ping_interval};
  }

  void send(const std::string_view data, const bool is_binary)
  {
    rep_->send(rep_.get(), data.data(), data.size(), is_binary ? UWSC_OP_BINARY : UWSC_OP_TEXT);
  }

  void send_text(const std::string_view data)
  {
    rep_->send(rep_.get(), data.data(), data.size(), UWSC_OP_TEXT);
  }

  void send_binary(const std::string_view data)
  {
    rep_->send(rep_.get(), data.data(), data.size(), UWSC_OP_BINARY);
  }

  void close(const int code, const std::string& reason = {}) noexcept
  {
    if (is_open_)
      rep_->send_close(rep_.get(), code, reason.c_str());
  }

  void ping()
  {
    rep_->ping(rep_.get());
  }

protected:
  virtual void handle_open() = 0;
  virtual void handle_message(std::string_view data, bool is_binary) = 0;
  virtual void handle_error(int code, std::string_view message) = 0;
  virtual void handle_close(int code, std::string_view reason) = 0;

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
