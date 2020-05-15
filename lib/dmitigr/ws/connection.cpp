// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#ifdef __GNUG__
// Disable some warnings of uWebSockets.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

#include "dmitigr/ws/basics.hpp"
#include "dmitigr/ws/connection.hpp"
#include <dmitigr/base/debug.hpp>
#include <dmitigr/net/net.hpp>

#include <uwebsockets/App.h>

namespace dmitigr::ws::detail {

/**
 * @brief The abstract representation of Listener_options.
 */
class iConnection {
public:
  virtual ~iConnection() = default;
  virtual std::string remote_ip_address() const = 0;
  virtual std::size_t buffered_amount() const = 0;
  virtual void send(std::string_view data, Data_format format) = 0;
  virtual void close(int code, std::string_view reason) = 0;
  virtual void abort() = 0;
  virtual bool is_closed() const = 0;
  virtual void event_loop_call_soon(std::function<void()> callback) = 0;
  virtual bool is_ssl() const noexcept = 0;
};

/**
 * @brief The representation of Listener_options.
 */
template<bool IsSsl>
class Conn final : public iConnection {
public:
  using Underlying_type = uWS::WebSocket<IsSsl, true>;

  Conn(const Conn&) = delete;
  Conn& operator=(const Conn&) = delete;
  Conn(Conn&&) = delete;
  Conn& operator=(Conn&&) = delete;

  explicit Conn(Underlying_type* const ws)
    : ws_{ws}
  {
    DMITIGR_ASSERT(ws_);
  }

  std::string remote_ip_address() const override
  {
    DMITIGR_ASSERT(ws_);
    const auto ip = net::Ip_address::from_binary(ws_->getRemoteAddress());
    return ip.to_string();
  }

  std::size_t buffered_amount() const override
  {
    DMITIGR_ASSERT(ws_);
    return ws_->getBufferedAmount();
  }

  void send(const std::string_view data, const Data_format format) override
  {
    DMITIGR_ASSERT(ws_);
    ws_->send(data, (format == Data_format::text) ? uWS::OpCode::TEXT : uWS::OpCode::BINARY);
  }

  void close(const int code, const std::string_view reason) override
  {
    DMITIGR_ASSERT(ws_);
    ws_->end(code, reason);
    ws_ = nullptr;
  }

  void abort() override
  {
    DMITIGR_ASSERT(ws_);
    ws_->close();
    ws_ = nullptr;
  }

  bool is_closed() const override
  {
    DMITIGR_ASSERT(ws_);
    return us_socket_is_closed(is_ssl(), reinterpret_cast<us_socket_t*>(ws_));
  }

  void event_loop_call_soon(std::function<void()> callback) override
  {
    DMITIGR_ASSERT(ws_);
    auto* const uss = reinterpret_cast<us_socket_t*>(ws_);
    us_socket_context_t* const uss_ctx = us_socket_context(is_ssl(), uss);
    us_loop_t* const uss_loop = us_socket_context_loop(is_ssl(), uss_ctx);
    auto* const loop = reinterpret_cast<uWS::Loop*>(uss_loop);
    loop->defer(callback);
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

DMITIGR_WS_INLINE void Connection::event_loop_call_soon(std::function<void()> callback)
{
  const std::lock_guard lg{mut_};
  if (rep_ && !rep_->is_closed())
    rep_->event_loop_call_soon(callback);
}

DMITIGR_WS_INLINE bool Connection::is_connected() const
{
  return rep_ && !rep_->is_closed();
}

DMITIGR_WS_INLINE std::string Connection::remote_ip_address() const
{
  DMITIGR_REQUIRE(rep_, std::logic_error);
  return rep_->remote_ip_address();
}

DMITIGR_WS_INLINE std::size_t Connection::buffered_amount() const
{
  DMITIGR_REQUIRE(rep_, std::logic_error);
  return rep_->buffered_amount();
}

DMITIGR_WS_INLINE void Connection::send(const std::string_view data, const Data_format format)
{
  DMITIGR_REQUIRE(rep_, std::logic_error);
  rep_->send(data, format);
}

DMITIGR_WS_INLINE void Connection::close(const int code, const std::string_view reason)
{
  if (rep_)
    rep_->close(code, reason);
  DMITIGR_ASSERT(!rep_);
}

DMITIGR_WS_INLINE void Connection::abort()
{
  if (rep_)
    rep_->abort();
  DMITIGR_ASSERT(!rep_);
}

} // namespace dmitigr::ws

#ifdef __GNUG__
#pragma GCC diagnostic pop
#endif
