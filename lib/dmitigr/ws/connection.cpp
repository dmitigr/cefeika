// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include "dmitigr/ws/basics.hpp"
#include "dmitigr/ws/connection.hpp"
#include "dmitigr/ws/util.hpp"
#include "dmitigr/ws/uwebsockets.hpp"
#include <dmitigr/net/net.hpp>

#include <cassert>

namespace dmitigr::ws::detail {

/// The abstract representation of Listener_options.
class iConnection {
public:
  virtual ~iConnection() = default;
  virtual Connection* connection() const = 0;
  virtual std::string remote_ip_address() const = 0;
  virtual std::string local_ip_address() const = 0;
  virtual std::size_t buffered_amount() const = 0;
  virtual bool send(std::string_view data, Data_format format) = 0;
  virtual void close(int code, std::string_view reason) = 0;
  virtual void abort() = 0;
  virtual bool is_closed() const = 0;
  virtual void event_loop_call_soon(std::function<void()> callback) = 0;
  virtual bool is_ssl() const noexcept = 0;
};

/// The representation of Listener_options.
template<bool IsSsl>
class Conn final : public iConnection {
public:
  using Underlying_type = uWS::WebSocket<IsSsl, true, Ws_data>;

  Conn(const Conn&) = delete;
  Conn& operator=(const Conn&) = delete;
  Conn(Conn&&) = delete;
  Conn& operator=(Conn&&) = delete;

  explicit Conn(Underlying_type* const ws)
    : ws_{ws}
  {
    assert(!is_closed());
  }

  Connection* connection() const override
  {
    auto* const data = ws_->getUserData();
    assert(data);
    return data->conn.get();
  }

  std::string remote_ip_address() const override
  {
    assert(!is_closed());
    const auto ip = net::Ip_address::from_binary(ws_->getRemoteAddress());
    return ip.to_string();
  }

  std::string local_ip_address() const override
  {
    assert(!is_closed());
    const auto ip = net::Ip_address::from_binary(detail::local_address(is_ssl(), reinterpret_cast<us_socket_t*>(ws_)));
    return ip.to_string();
  }

  std::size_t buffered_amount() const override
  {
    assert(!is_closed());
    return ws_->getBufferedAmount();
  }

  bool send(const std::string_view data, const Data_format format) override
  {
    assert(!is_closed());
    return ws_->send(data, (format == Data_format::text) ? uWS::OpCode::TEXT : uWS::OpCode::BINARY);
  }

  void close(const int code, const std::string_view reason) override
  {
    assert(!is_closed());
    ws_->end(code, reason);
    ws_ = nullptr;
  }

  void abort() override
  {
    assert(!is_closed());
    ws_->close();
    ws_ = nullptr;
  }

  bool is_closed() const override
  {
    assert(ws_);
    return us_socket_is_closed(is_ssl(), reinterpret_cast<us_socket_t*>(ws_));
  }

  void event_loop_call_soon(std::function<void()> callback) override
  {
    assert(!is_closed());
    auto* const uss = reinterpret_cast<us_socket_t*>(ws_);
    us_socket_context_t* const uss_ctx = us_socket_context(is_ssl(), uss);
    us_loop_t* const uss_loop = us_socket_context_loop(is_ssl(), uss_ctx);
    auto* const loop = reinterpret_cast<uWS::Loop*>(uss_loop);
    loop->defer(std::move(callback));
  }

  bool is_ssl() const noexcept override
  {
    return IsSsl;
  }

private:
  Underlying_type* ws_;
};

} // namespace dmitigr::ws::detail

namespace dmitigr::ws {

DMITIGR_WS_INLINE Connection::~Connection() = default;
DMITIGR_WS_INLINE Connection::Connection() = default;

DMITIGR_WS_INLINE bool Connection::event_loop_call_soon(std::function<void()> callback)
{
  const std::lock_guard lg{mut_};
  if (rep_ && !rep_->is_closed()) {
    rep_->event_loop_call_soon(std::move(callback));
    return true;
  } else
    return false;
}

DMITIGR_WS_INLINE bool Connection::is_connected() const
{
  return rep_ && !rep_->is_closed();
}

DMITIGR_WS_INLINE std::string Connection::remote_ip_address() const
{
  assert(is_connected());
  return rep_->remote_ip_address();
}

DMITIGR_WS_INLINE std::string Connection::local_ip_address() const
{
  assert(is_connected());
  return rep_->local_ip_address();
}

DMITIGR_WS_INLINE std::size_t Connection::buffered_amount() const
{
  assert(is_connected());
  return rep_->buffered_amount();
}

DMITIGR_WS_INLINE void Connection::send(const std::string_view data, const Data_format format)
{
  assert(is_connected());
  rep_->send(data, format);
}

DMITIGR_WS_INLINE void Connection::send_text(const std::string_view data)
{
  send(data, Data_format::text);
}

DMITIGR_WS_INLINE void Connection::send_binary(const std::string_view data)
{
  send(data, Data_format::binary);
}

DMITIGR_WS_INLINE void Connection::close(const int code, const std::string_view reason)
{
  if (is_connected())
    rep_->close(code, reason);
  assert(!is_connected());
}

DMITIGR_WS_INLINE void Connection::abort()
{
  if (is_connected())
    rep_->abort();
  assert(!is_connected());
}

} // namespace dmitigr::ws
