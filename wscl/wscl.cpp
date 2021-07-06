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

#include "wscl.hpp"
#include "../assert.hpp"
#include "../3rdparty/uwsc/uwsc.h"

#include <cstring>
#include <stdexcept>

namespace dmitigr::wscl {

DMITIGR_WSCL_INLINE Connection_options& Connection_options::url(std::string value)
{
  url_ = std::move(value);
  return *this;
}

DMITIGR_WSCL_INLINE const std::string& Connection_options::url() const noexcept
{
  return url_;
}

DMITIGR_WSCL_INLINE Connection_options& Connection_options::ping_interval(const std::chrono::seconds value)
{
  ping_interval_ = value;
  return *this;
}

DMITIGR_WSCL_INLINE std::chrono::seconds Connection_options::ping_interval() const noexcept
{
  return ping_interval_;
}

DMITIGR_WSCL_INLINE Connection_options& Connection_options::extra_header(const std::string_view name,
  const std::string_view value)
{
  extra_headers_.append(name).append(":").append(value).append("\r\n");
  return *this;
}

DMITIGR_WSCL_INLINE const std::string& Connection_options::extra_headers() const noexcept
{
  return extra_headers_;
}

DMITIGR_WSCL_INLINE Connection::~Connection()
{
  close(UWSC_CLOSE_STATUS_NORMAL);
}

DMITIGR_WSCL_INLINE Connection::Connection(void* const loop, Options options)
  : options_{std::move(options)}
{
  DMITIGR_CHECK_ARG(loop);

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

DMITIGR_WSCL_INLINE bool Connection::is_open() const noexcept
{
  return is_open_;
}

DMITIGR_WSCL_INLINE auto Connection::options() const noexcept -> const Options&
{
  return options_;
}

DMITIGR_WSCL_INLINE void Connection::set_ping_interval(const std::chrono::seconds interval)
{
  DMITIGR_CHECK(is_open_);
  options_.ping_interval(interval);
  rep_->ping_interval = interval.count();
}

DMITIGR_WSCL_INLINE void Connection::send(const std::string_view data, const bool is_binary)
{
  DMITIGR_CHECK(is_open_);
  rep_->send(rep_.get(), data.data(), data.size(), is_binary ? UWSC_OP_BINARY : UWSC_OP_TEXT);
}

DMITIGR_WSCL_INLINE void Connection::send_text(const std::string_view data)
{
  DMITIGR_CHECK(is_open_);
  rep_->send(rep_.get(), data.data(), data.size(), UWSC_OP_TEXT);
}

DMITIGR_WSCL_INLINE void Connection::send_binary(const std::string_view data)
{
  DMITIGR_CHECK(is_open_);
  rep_->send(rep_.get(), data.data(), data.size(), UWSC_OP_BINARY);
}

DMITIGR_WSCL_INLINE void Connection::ping()
{
  DMITIGR_CHECK(is_open_);
  rep_->ping(rep_.get());
}

DMITIGR_WSCL_INLINE void Connection::close(const int code, const std::string& reason) noexcept
{
  if (is_open_)
    rep_->send_close(rep_.get(), code, reason.c_str());
}

DMITIGR_WSCL_INLINE void Connection::handle_open__(uwsc_client* const cl) noexcept
{
  auto* const s = self(cl);
  s->is_open_ = true;
  s->handle_open();
}

DMITIGR_WSCL_INLINE void Connection::handle_message__(uwsc_client* const cl, void* const data,
  const std::size_t size, const bool binary) noexcept
{
  self(cl)->handle_message({static_cast<char*>(data), size}, binary);
}

DMITIGR_WSCL_INLINE void Connection::handle_error__(uwsc_client* const cl, const int code, const char* const message) noexcept
{
  auto* const s = self(cl);
  s->is_open_ = false;
  s->handle_error(code, {message, std::strlen(message)});
}

DMITIGR_WSCL_INLINE void Connection::handle_close__(uwsc_client* const cl, const int code, const char* const reason) noexcept
{
  auto* const s = self(cl);
  s->is_open_ = false;
  s->handle_close(code, {reason, std::strlen(reason)});
}

DMITIGR_WSCL_INLINE Connection* Connection::self(uwsc_client* const cl) noexcept
{
  DMITIGR_ASSERT(cl);
  auto* const self = static_cast<Connection*>(cl->ext);
  DMITIGR_ASSERT(self);
  return self;
}

} // namespace dmitigr::wscl
