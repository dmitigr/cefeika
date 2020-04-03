// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include "dmitigr/ws/connection.hpp"
#include "dmitigr/ws/http_request.hpp"
#include "dmitigr/ws/listener.hpp"
#include "dmitigr/ws/listener_options.hpp"
#include "dmitigr/util/debug.hpp"

#include <uwebsockets/App.h>

#include <limits>

//#define DMITIGR_WS_DEBUG

#ifdef DMITIGR_WS_DEBUG
#include <iostream>
#endif
#include <stdexcept>

namespace dmitigr::ws::detail {

/**
 * @brief The abstract representation of Listener.
 */
class iListener {
public:
  using Options = Listener_options;
  virtual ~iListener() = default;
  virtual const Options& options() const = 0;
  virtual bool is_listening() const = 0;
  virtual void listen() = 0;
  virtual void close() = 0;
};

/**
 * @brief The representation of Listener.
 */
template<bool IsSsl>
class Lstnr final : public iListener {
public:
  explicit Lstnr(Listener* const listener, const Options& options)
    : listener_{listener}
  {
    options_ = options.to_listener_options();
    DMITIGR_ASSERT(listener_ && options_);
  }

  const Options& options() const override
  {
    return *options_;
  }

  bool is_listening() const override
  {
    return loop_;
  }

  void listen() override
  {
    DMITIGR_REQUIRE(!is_listening(), std::logic_error);

    using App = uWS::TemplatedApp<IsSsl>;

    const auto behavior = [this]
    {
      typename App::WebSocketBehavior result;

      const auto idle_timeout = std::chrono::duration_cast<std::chrono::seconds>(
        options().idle_timeout().value_or(std::chrono::milliseconds::zero()));

      const auto max_payload_size = options().max_payload_size();
      using Ws_payload_size = decltype(max_payload_size);
      using Uws_payload_size = decltype(result.maxPayloadLength);
      static_assert(sizeof(Ws_payload_size) >= sizeof(Uws_payload_size));
      const auto max_payload_size_max = static_cast<Ws_payload_size>(std::numeric_limits<Uws_payload_size>::max());
      DMITIGR_ASSERT(max_payload_size <= max_payload_size_max);

      result.compression = uWS::DISABLED;
      result.maxPayloadLength = static_cast<Uws_payload_size>(max_payload_size);
      result.idleTimeout = idle_timeout.count(); // eternity
      result.maxBackpressure = std::numeric_limits<decltype(result.maxPayloadLength)>::max();

      result.open = [this](auto* const ws, auto* const req)
      {
#ifdef DMITIGR_WS_DEBUG
        std::clog << "dmitigr::ws: .open emitted" << std::endl;
#endif
        auto* const data = static_cast<Ws_data*>(ws->getUserData());
        DMITIGR_ASSERT(data);
        const detail::iHttp_request handshake{req, ws->getRemoteAddress()};
        data->conn = listener_->make_connection(&handshake);
        if (data->conn)
          data->conn->rep_ = std::make_unique<detail::Conn<IsSsl>>(ws);
        else
          ws->end(1011, "internal error");
      };

      result.message = [](auto* const ws, const std::string_view message, const uWS::OpCode oc)
      {
        auto* const data = static_cast<Ws_data*>(ws->getUserData());
        DMITIGR_ASSERT(data && data->conn);
        const auto message_format = (oc == uWS::OpCode::TEXT) ? Data_format::text : Data_format::binary;
        data->conn->handle_message(message, message_format);
      };

      result.drain = [](auto* const ws)
      {
        (void)ws;
#ifdef DMITIGR_WS_DEBUG
        std::clog << "dmitigr::ws: .drain emitted. Buffered amount is " << ws->getBufferedAmount() << std::endl;
#endif
      };

      result.ping = [](auto* const /*ws*/)
      {
#ifdef DMITIGR_WS_DEBUG
        std::clog << "dmitigr::ws: .ping emitted" << std::endl;
#endif
      };

      result.pong = [](auto* const /*ws*/)
      {
#ifdef DMITIGR_WS_DEBUG
        std::clog << "dmitigr::ws: .pong emitted" << std::endl;
#endif
      };

      result.close = [](auto* const ws, const int code, const std::string_view reason)
      {
#ifdef DMITIGR_WS_DEBUG
        std::clog << "dmitigr::ws: .close emitted" << std::endl;
#endif
        auto* const data = static_cast<Ws_data*>(ws->getUserData());
        DMITIGR_ASSERT(data);
        // If connection is closed from .open, then (data->conn == nullptr) in such a case.
        if (data->conn) {
          data->conn->handle_close(code, reason);
          {
            const std::lock_guard lg{data->conn->mut_};
            data->conn->rep_.reset();
          }
          data->conn.reset();
        }
      };

      return result;
    }; // behaviour

    const auto host = options().endpoint_id()->net_address().value();
    const auto port = options().endpoint_id()->net_port().value();
    loop_ = uWS::Loop::get();
    App{}.template ws<Ws_data>("/*", behavior())
      .listen(host, port,
        [this](auto* const listening_socket)
        {
          if (listening_socket)
            listening_socket_ = listening_socket;
          else
            throw std::runtime_error{"WebSocket listener is failed to start"};
        }).run();
  }

  void close() override
  {
    if (loop_) {
      DMITIGR_ASSERT(listening_socket_);
      loop_->defer([this]
      {
        us_listen_socket_close(IsSsl, listening_socket_);
        listening_socket_ = nullptr;
      });
      loop_ = nullptr;
    }
  }

private:
  // The data associated with every WebSocket.
  struct Ws_data final {
    std::shared_ptr<Connection> conn;
  };

  Listener* listener_;
  std::unique_ptr<Listener_options> options_;
  us_listen_socket_t* listening_socket_{};
  uWS::Loop* loop_{};
};

using Non_ssl_listener = Lstnr<false>;
using Ssl_listener = Lstnr<true>;

} // namespace dmitigr::ws::detail

namespace dmitigr::ws {

DMITIGR_WS_INLINE Listener::~Listener() = default;
DMITIGR_WS_INLINE Listener::Listener() = default;

DMITIGR_WS_INLINE Listener::Listener(const Options& options)
  : rep_{std::make_unique<detail::Non_ssl_listener>(this, options)}
{
  DMITIGR_ASSERT(rep_);
}

DMITIGR_WS_INLINE auto Listener::options() const -> const Options&
{
  return rep_->options();
}

DMITIGR_WS_INLINE bool Listener::is_listening() const
{
  return rep_->is_listening();
}

DMITIGR_WS_INLINE void Listener::listen()
{
  rep_->listen();
}

DMITIGR_WS_INLINE void Listener::close()
{
  rep_->close();
}

} // namespace dmitigr::ws

#ifdef DMITIGR_WS_DEBUG
#undef DMITIGR_WS_DEBUG
#endif
