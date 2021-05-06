// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include "dmitigr/ws/connection.hpp"
#include "dmitigr/ws/http_io.hpp"
#include "dmitigr/ws/http_request.hpp"
#include "dmitigr/ws/listener.hpp"
#include "dmitigr/ws/listener_options.hpp"
#include "dmitigr/ws/timer.hpp"
#include "dmitigr/ws/util.hpp"
#include "dmitigr/ws/uwebsockets.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <vector>
#include <utility>

//#define DMITIGR_WS_DEBUG

#ifdef DMITIGR_WS_DEBUG
#include <iostream>
#endif
#include <stdexcept>

namespace dmitigr::ws::detail {

/// The abstract representation of Listener.
class iListener {
public:
  using Options = Listener_options;

  virtual ~iListener() = default;
  virtual const Options& options() const = 0;
  virtual bool is_listening() const = 0;
  virtual void listen() = 0;
  virtual void close() = 0;
  virtual void close_connections(int code, std::string reason) = 0;

  virtual void event_loop_call_soon(std::function<void()> callback) = 0;
  virtual void for_each(std::function<void(Connection*)> callback) = 0;

  virtual std::size_t timer_count() const = 0;
  virtual std::optional<std::size_t> timer_index(std::string_view name) const = 0;
  virtual iTimer& add_timer(std::string name) = 0;
  virtual void remove_timer(std::string_view name) = 0;
  virtual iTimer* timer(std::string_view name) const = 0;
  virtual iTimer& timer(std::size_t pos) const = 0;
  virtual const std::string& timer_name(std::size_t pos) const = 0;
};

/// The representation of Listener.
template<bool IsSsl>
class Lstnr final : public iListener {
public:
  explicit Lstnr(Listener* const listener, const Options& options)
    : listener_{listener}
    , loop_{uWS::Loop::get()}
  {
    options_ = options.to_listener_options();
    assert(listener_ && loop_ && options_);
  }

  const Options& options() const override
  {
    return *options_;
  }

  bool is_listening() const override
  {
    return listening_socket_;
  }

  void listen() override
  {
    assert(!is_listening());

    using App = uWS::TemplatedApp<IsSsl>;

    const auto ws_behavior = [this]
    {
      namespace chrono = std::chrono;
      typename App::WebSocketBehavior<Ws_data> result;

      result.compression = uWS::DISABLED;

      using Timeout = decltype(result.idleTimeout);
      const auto idle_timeout = chrono::duration_cast<chrono::seconds>(
        options().idle_timeout().value_or(chrono::milliseconds::zero()));
      constexpr auto max_idle_timeout = std::numeric_limits<Timeout>::max();
      result.idleTimeout = std::min(max_idle_timeout, static_cast<Timeout>(idle_timeout.count()));

      const auto max_payload_size = options().max_payload_size();
      assert(max_payload_size <= std::numeric_limits<int>::max());
      result.maxPayloadLength = static_cast<decltype(result.maxPayloadLength)>(max_payload_size);

      const auto max_buffered_amount = options().max_buffered_amount();
      assert(max_buffered_amount <= std::numeric_limits<int>::max());
      result.maxBackpressure = static_cast<decltype(result.maxBackpressure)>(max_buffered_amount);

      result.upgrade = [this](uWS::HttpResponse<IsSsl>* const res, uWS::HttpRequest* const req,
        us_socket_context_t* const ctx)
      {
#ifdef DMITIGR_WS_DEBUG
        std::clog << "dmitigr::ws: .upgrade emitted" << std::endl;
#endif
        const iHttp_request request{req, res->getRemoteAddress(),
          local_address(IsSsl, reinterpret_cast<us_socket_t*>(res))};
        const auto io = std::make_shared<iHttp_io_templ<IsSsl>>(res);
        Ws_data data{listener_->handle_handshake(request, io)};
        if (data.conn) {
          if (!io->is_valid())
            throw std::runtime_error{"invalid instance of dmitigr::ws::Http_io"};
          const auto sec_ws_key = request.header("sec-websocket-key");
          const auto sec_ws_protocol = request.header("sec-websocket-protocol");
          const auto sec_ws_extensions = request.header("sec-websocket-extensions");
          if (!io->is_abort_handler_set()) {
            if (io->is_response_handler_set())
              throw std::logic_error{"attempt to complete WebSocket handshake"
                " implicitly after setting a response handler"};
            res->upgrade(std::move(data),
              sec_ws_key,
              sec_ws_protocol,
              sec_ws_extensions,
              ctx);
          } else { // deferred handshake
            io->ws_data_ = std::move(data);
            io->sec_ws_key_ = sec_ws_key;
            io->sec_ws_protocol_ = sec_ws_protocol;
            io->sec_ws_extensions_ = sec_ws_extensions;
            io->ctx_ = ctx;
          }
        } else if (io->is_valid() && !io->is_abort_handler_set()) { // implicit rejection
          io->send_status(500, "Internal Error");
          io->end();
        }
      };

      result.open = [this](auto* const ws)
      {
#ifdef DMITIGR_WS_DEBUG
        std::clog << "dmitigr::ws: .open emitted" << std::endl;
#endif
        auto* const data = ws->getUserData();
        assert(data);
        if (data->conn) {
          data->conn->rep_ = std::make_unique<Conn<IsSsl>>(ws);
          connections_.emplace_back(static_cast<Conn<IsSsl>*>(data->conn->rep_.get()));
        } else
          ws->end(1011, "internal error");
      };

      result.message = [](auto* const ws, const std::string_view message, const uWS::OpCode oc)
      {
        auto* const data = ws->getUserData();
        assert(data && data->conn);
        const auto message_format = (oc == uWS::OpCode::TEXT) ? Data_format::text : Data_format::binary;
        data->conn->handle_message(message, message_format);
      };

      result.drain = [](auto* const ws)
      {
#ifdef DMITIGR_WS_DEBUG
        std::clog << "dmitigr::ws: .drain emitted. Buffered amount is " << ws->getBufferedAmount() << std::endl;
#endif
        auto* const data = ws->getUserData();
        assert(data && data->conn);
        data->conn->handle_drain();
      };

      result.ping = [](auto* const /*ws*/, const std::string_view)
      {
#ifdef DMITIGR_WS_DEBUG
        std::clog << "dmitigr::ws: .ping emitted" << std::endl;
#endif
      };

      result.pong = [](auto* const /*ws*/, const std::string_view)
      {
#ifdef DMITIGR_WS_DEBUG
        std::clog << "dmitigr::ws: .pong emitted" << std::endl;
#endif
      };

      result.close = [this](auto* const ws, const int code, const std::string_view reason)
      {
#ifdef DMITIGR_WS_DEBUG
        std::clog << "dmitigr::ws: .close emitted" << std::endl;
#endif
        auto* const data = ws->getUserData();
        assert(data);
        /*
         * If connection is closed from .open or .upgrade,
         * then (data->conn == nullptr) in such a case.
         */
        if (data->conn) {
          data->conn->handle_close(code, reason);
          if (!close_connections_called_) {
            auto* conn = static_cast<Conn<IsSsl>*>(data->conn->rep_.get());
            const auto b = cbegin(connections_);
            const auto e = cend(connections_);
            const auto i = std::find_if(b, e, [conn](const auto p) { return p == conn; });
            assert(i != e);
            connections_.erase(i);
          }
          {
            const std::lock_guard lg{data->conn->mut_};
            data->conn->rep_.reset();
          }
          data->conn.reset();
        }
        assert(!data->conn);
      };

      return result;
    }; // ws_behaviour

    const auto host = options().endpoint().net_address().value();
    const auto port = options().endpoint().net_port().value();

    // This is for Windows where std::filesystem::path::c_str() returns const wchar_t*.
    std::string ssl_private_key_file,
      ssl_certificate_file,
      ssl_dh_parameters_file,
      ssl_certificate_authority_file;
    App app = [this, &ssl_private_key_file, &ssl_certificate_file, &ssl_dh_parameters_file, &ssl_certificate_authority_file]
    {
      uWS::SocketContextOptions socket_options{};
      if (options().is_ssl_enabled()) {
        assert(IsSsl);
        if (const auto& value = options().ssl_private_key_file()) {
          ssl_private_key_file = value->string();
          socket_options.key_file_name = ssl_private_key_file.c_str();
        }
        if (const auto& value = options().ssl_certificate_file()) {
          ssl_certificate_file = value->string();
          socket_options.cert_file_name = ssl_certificate_file.c_str();
        }
        if (const auto& value = options().ssl_pem_file_password())
          socket_options.passphrase = value->c_str();
        if (const auto& value = options().ssl_dh_parameters_file()) {
          ssl_dh_parameters_file = value->string();
          socket_options.dh_params_file_name = ssl_dh_parameters_file.c_str();
        }
        if (const auto& value = options().ssl_certificate_authority_file()) {
          ssl_certificate_authority_file = value->string();
          socket_options.ca_file_name = ssl_certificate_authority_file.c_str();
        }
        socket_options.ssl_prefer_low_memory_usage = 0;
      }
      return App{socket_options};
    }();
    app.template ws<Ws_data>("/*", ws_behavior());
    if (options().is_http_enabled()) {
      app.any("/*", [this](auto* const res, auto* const req)
      {
        const iHttp_request request{req, res->getRemoteAddress(),
          local_address(IsSsl, reinterpret_cast<us_socket_t*>(res))};
        const auto io = std::make_shared<iHttp_io_templ<IsSsl>>(res);
        listener_->handle_request(request, io);
        if (io->is_valid() && !io->is_abort_handler_set())
          io->abort();
      });
    }
    app.listen(host, port, [this](auto* const listening_socket)
    {
      if (listening_socket)
        listening_socket_ = listening_socket;
      else
        throw std::runtime_error{"WebSocket listener is failed to start"};
    });
    app.run();
  }

  void close() override
  {
    loop_->defer([this]
    {
      if (is_listening()) {
        assert(listening_socket_);
        us_listen_socket_close(IsSsl, listening_socket_);
        listening_socket_ = nullptr;
      }
    });
  }

  void close_connections(const int code, std::string reason) override
  {
    loop_->defer([this, code, reason = std::move(reason)]
    {
      if (is_listening()) {
        struct Guard {
          Guard(Lstnr& l) : l_{l} { l_.close_connections_called_ = true; }
          ~Guard() { l_.close_connections_called_ = false; }
        private:
          Lstnr& l_;
        } guard{*this};

        for (auto* const conn : connections_) {
          assert(conn);
          conn->close(code, reason);
        }
        connections_.clear();
      }
    });
  }

  void event_loop_call_soon(std::function<void()> callback) override
  {
    loop_->defer(std::move(callback));
  }

  void for_each(std::function<void(Connection*)> callback) override
  {
    loop_->defer([this, callback = std::move(callback)]
    {
      for (auto* const conn : connections_) {
        assert(conn);
        callback(conn->connection());
      }
    });
  }

  std::size_t timer_count() const override
  {
    return timers_.size();
  }

  std::optional<std::size_t> timer_index(const std::string_view name) const override
  {
    const auto b = cbegin(timers_);
    const auto e = cend(timers_);
    const auto i = std::find_if(b, e, [&name](const auto& p) { return p.first == name; });
    return (i != e) ? std::make_optional(i - b) : std::nullopt;
  }

  iTimer& add_timer(std::string name) override
  {
    assert(loop_);
    assert(!timer(name));
    return timers_.emplace_back(std::move(name), reinterpret_cast<us_loop_t*>(loop_)).second;
  }

  void remove_timer(const std::string_view name) override
  {
    if (const auto i = timer_index(name)) {
      const auto b = cbegin(timers_);
      using Diff = typename decltype(b)::difference_type;
      timers_.erase(b + static_cast<Diff>(*i));
    }
  }

  iTimer* timer(const std::string_view name) const override
  {
    const auto i = timer_index(name);
    return i ? &(timers_[*i].second) : nullptr;
  }

  iTimer& timer(const std::size_t pos) const override
  {
    assert(pos < timer_count());
    return timers_[pos].second;
  }

  const std::string& timer_name(const std::size_t pos) const override
  {
    assert(pos < timer_count());
    return timers_[pos].first;
  }

private:
  Listener* listener_;
  uWS::Loop* loop_;
  std::unique_ptr<Listener_options> options_;
  us_listen_socket_t* listening_socket_{};
  mutable std::vector<std::pair<std::string, iTimer>> timers_;

  bool close_connections_called_{};
  std::vector<Conn<IsSsl>*> connections_;
};

using Non_ssl_listener = Lstnr<false>;
#ifdef DMITIGR_CEFEIKA_OPENSSL
using Ssl_listener = Lstnr<true>;
#endif

} // namespace dmitigr::ws::detail

namespace dmitigr::ws {

DMITIGR_WS_INLINE Listener::~Listener() = default;
DMITIGR_WS_INLINE Listener::Listener() = default;

DMITIGR_WS_INLINE Listener::Listener(const Options& options)
{
#ifdef DMITIGR_CEFEIKA_OPENSSL
  if (options.is_ssl_enabled())
    rep_ = std::make_unique<detail::Ssl_listener>(this, options);
  else
#endif
    rep_ = std::make_unique<detail::Non_ssl_listener>(this, options);

  assert(rep_);
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

DMITIGR_WS_INLINE void Listener::close_connections(const int code, std::string reason)
{
  rep_->close_connections(code, std::move(reason));
}

DMITIGR_WS_INLINE void Listener::event_loop_call_soon(std::function<void()> callback)
{
  rep_->event_loop_call_soon(std::move(callback));
}

DMITIGR_WS_INLINE void Listener::for_each(std::function<void(Connection*)> callback)
{
  rep_->for_each(std::move(callback));
}

DMITIGR_WS_INLINE std::size_t Listener::timer_count() const
{
  return rep_->timer_count();
}

DMITIGR_WS_INLINE std::optional<std::size_t> Listener::timer_index(const std::string_view name) const
{
  return rep_->timer_index(name);
}

DMITIGR_WS_INLINE Timer& Listener::add_timer(std::string name)
{
  return rep_->add_timer(std::move(name));
}

DMITIGR_WS_INLINE void Listener::remove_timer(const std::string_view name)
{
  rep_->remove_timer(name);
}

DMITIGR_WS_INLINE Timer* Listener::timer(const std::string_view name) const
{
  return rep_->timer(name);
}

DMITIGR_WS_INLINE Timer& Listener::timer(const std::size_t pos) const
{
  return rep_->timer(pos);
}

DMITIGR_WS_INLINE const std::string& Listener::timer_name(const std::size_t pos) const
{
  return rep_->timer_name(pos);
}

DMITIGR_WS_INLINE void Listener::handle_request(const Http_request&, std::shared_ptr<Http_io>)
{
}

} // namespace dmitigr::ws

#ifdef DMITIGR_WS_DEBUG
#undef DMITIGR_WS_DEBUG
#endif
