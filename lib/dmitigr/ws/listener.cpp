// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include "dmitigr/ws/connection.hpp"
#include "dmitigr/ws/http_io.hpp"
#include "dmitigr/ws/http_request.hpp"
#include "dmitigr/ws/listener.hpp"
#include "dmitigr/ws/listener_options.hpp"
#include "dmitigr/ws/timer.hpp"
#include <dmitigr/base/debug.hpp>

#include <uwebsockets/App.h>

#include <limits>
#include <vector>
#include <utility>

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

  virtual std::size_t timer_count() const = 0;
  virtual std::optional<std::size_t> timer_index(std::string_view name) const = 0;
  virtual std::size_t timer_index_throw(std::string_view name) const = 0;
  virtual iTimer& add_timer(std::string name) = 0;
  virtual void remove_timer(std::string_view name) = 0;
  virtual iTimer* timer(std::string_view name) const = 0;
  virtual iTimer& timer(std::size_t pos) const = 0;
  virtual const std::string& timer_name(std::size_t pos) const = 0;
};

/**
 * @brief The representation of Listener.
 */
template<bool IsSsl>
class Lstnr final : public iListener {
public:
  explicit Lstnr(Listener* const listener, const Options& options)
    : listener_{listener}
    , loop_{uWS::Loop::get()}
  {
    options_ = options.to_listener_options();
    DMITIGR_ASSERT(listener_ && loop_ && options_);
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
    DMITIGR_REQUIRE(!is_listening(), std::logic_error);

    using App = uWS::TemplatedApp<IsSsl>;

    const auto ws_behavior = [this]
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
        const iHttp_request handshake{req, ws->getRemoteAddress(),
          local_address(IsSsl, reinterpret_cast<us_socket_t*>(ws))};
        data->conn = listener_->make_connection(handshake);
        if (data->conn)
          data->conn->rep_ = std::make_unique<Conn<IsSsl>>(ws);
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
    }; // ws_behaviour

    const auto host = options().endpoint().net_address().value();
    const auto port = options().endpoint().net_port().value();

    App app = [this]
    {
      us_socket_context_options_t socket_options{};
      if (options().is_ssl_enabled()) {
        DMITIGR_ASSERT(IsSsl);
        if (const auto value = options().ssl_private_key_file())
          socket_options.key_file_name = value->c_str();
        if (const auto value = options().ssl_certificate_file())
          socket_options.cert_file_name = value->c_str();
        if (const auto value = options().ssl_pem_file_password())
          socket_options.passphrase = value->c_str();
        if (const auto value = options().ssl_dh_parameters_file())
          socket_options.dh_params_file_name = value->c_str();
        if (const auto value = options().ssl_certificate_authority_file())
          socket_options.ca_file_name = value->c_str();
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
        if (!io->is_response_handler_set() || !io->is_abort_handler_set()) {
          io->rep_ = nullptr;
          DMITIGR_ASSERT(!io->is_valid());
        }
        if (!io->is_abort_handler_set())
          throw std::runtime_error{"The overriding of "
              "Listener::handle_request() didn't set the abort handler"};
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
    if (is_listening()) {
      DMITIGR_ASSERT(listening_socket_);
      loop_->defer([this]
      {
        us_listen_socket_close(IsSsl, listening_socket_);
        listening_socket_ = nullptr;
      });
    }
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

  std::size_t timer_index_throw(const std::string_view name) const override
  {
    const auto result = timer_index(name);
    DMITIGR_REQUIRE(result, std::out_of_range,
      "dmitigr::ws::Listener: no timer \"" + std::string{name} + "\"");
    return *result;
  }

  iTimer& add_timer(std::string name) override
  {
    DMITIGR_ASSERT(loop_);
    DMITIGR_REQUIRE(!timer(name), std::logic_error);
    return timers_.emplace_back(std::move(name), reinterpret_cast<us_loop_t*>(loop_)).second;
  }

  void remove_timer(const std::string_view name) override
  {
    if (const auto i = timer_index(name))
      timers_.erase(cbegin(timers_) + *i);
  }

  iTimer* timer(const std::string_view name) const override
  {
    const auto i = timer_index(name);
    return i ? &(timers_[*i].second) : nullptr;
  }

  iTimer& timer(const std::size_t pos) const override
  {
    DMITIGR_REQUIRE(pos < timer_count(), std::out_of_range);
    return timers_[pos].second;
  }

  const std::string& timer_name(const std::size_t pos) const override
  {
    DMITIGR_REQUIRE(pos < timer_count(), std::out_of_range);
    return timers_[pos].first;
  }

private:
  // The data associated with every WebSocket.
  struct Ws_data final {
    std::shared_ptr<Connection> conn;
  };

  Listener* listener_;
  uWS::Loop* loop_;
  std::unique_ptr<Listener_options> options_;
  us_listen_socket_t* listening_socket_{};
  mutable std::vector<std::pair<std::string, iTimer>> timers_;
};

using Non_ssl_listener = Lstnr<false>;
#ifdef DMITIGR_CEFEIKA_WITH_OPENSSL
using Ssl_listener = Lstnr<true>;
#endif

} // namespace dmitigr::ws::detail

namespace dmitigr::ws {

DMITIGR_WS_INLINE Listener::~Listener() = default;
DMITIGR_WS_INLINE Listener::Listener() = default;

DMITIGR_WS_INLINE Listener::Listener(const Options& options)
{
#ifdef DMITIGR_CEFEIKA_WITH_OPENSSL
  if (options.is_ssl_enabled())
    rep_ = std::make_unique<detail::Ssl_listener>(this, options);
  else
#endif
    rep_ = std::make_unique<detail::Non_ssl_listener>(this, options);

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

DMITIGR_WS_INLINE std::size_t Listener::timer_count() const
{
  return rep_->timer_count();
}

DMITIGR_WS_INLINE std::optional<std::size_t> Listener::timer_index(const std::string_view name) const
{
  return rep_->timer_index(name);
}

DMITIGR_WS_INLINE std::size_t Listener::timer_index_throw(const std::string_view name) const
{
  return rep_->timer_index_throw(name);
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

DMITIGR_WS_INLINE void Listener::handle_request(const ws::Http_request&,
  std::shared_ptr<ws::Http_io>)
{
}

} // namespace dmitigr::ws

#ifdef DMITIGR_WS_DEBUG
#undef DMITIGR_WS_DEBUG
#endif
