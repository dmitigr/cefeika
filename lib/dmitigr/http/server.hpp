// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_SERVER_HPP
#define DMITIGR_HTTP_SERVER_HPP

#include "dmitigr/http/connection.hpp"
#include "dmitigr/http/errc.hpp"
#include "dmitigr/http/types_fwd.hpp"
#include <dmitigr/net/listener.hpp>
#include <dmitigr/os/exceptions.hpp>

#include <string>
#include <type_traits>

namespace dmitigr::http {

/// A HTTP server connection.
class Server_connection final : public Connection {
public:
  /// @see Connection::is_server().
  bool is_server() const override
  {
    return true;
  }

  /**
   * @par Requires
   * `(is_head_received() && is_body_received())`.
   */
  template<class StatusCode>
  void send_start(const StatusCode code)
  {
    DMITIGR_REQUIRE(is_head_received() && is_body_received(), std::logic_error);

    static_assert(std::is_same_v<StatusCode, Server_infoc> ||
      std::is_same_v<StatusCode, Server_succ> ||
      std::is_same_v<StatusCode, Server_redirc> ||
      std::is_same_v<StatusCode, Server_errc>, "used invalid type for status code");

    const char* const literal = to_literal(code);
    DMITIGR_REQUIRE(literal, std::invalid_argument);
    std::string line{"HTTP/1.1 "};
    line.append(std::to_string(static_cast<int>(code))).append(" ");
    line.append(literal).append("\r\n");
    send_start__(line);
  }

private:
  friend Listener;
  using Connection::Connection;
};

/**
 * @brief HTTP Listener options.
 */
class Listener_options final {
public:
#ifdef _WIN32
  /// @see net::Listener_options.
  explicit Listener_options(std::string pipe_name)
    : listener_options_{std::move(pipe_name)}
  {}
#else
  /// @see net::Listener_options.
  Listener_options(std::filesystem::path path, int backlog)
    : listener_options_{std::move(path), backlog}
  {}
#endif
  /// @see net::Listener_options.
  Listener_options(std::string address, int port, int backlog)
    : listener_options_{std::move(address), port, backlog}
  {}

  /// @returns A new instance of the Listener initialized with this instance.
  Listener make_listener() const;

  /// @see net::Listener_options.
  const net::Endpoint& endpoint() const
  {
    return listener_options_.endpoint();
  }

  /// @see net::Listener_options.
  std::optional<int> backlog() const
  {
    return listener_options_.backlog();
  }

private:
  friend Listener;
  net::Listener_options listener_options_;
};

/**
 * @brief HTTP listener.
 */
class Listener final {
public:
  /// An alias of Listener_options.
  using Options = Listener_options;

  /// The constructor.
  explicit Listener(Options options)
    : listener_{net::Listener::make(options.listener_options_)}
    , options_{std::move(options)}
  {}

  /// @returns The listener options.
  const Options& options() const
  {
    return options_;
  }

  /// @see net::Listener::is_listening().
  bool is_listening() const
  {
    return listener_->is_listening();
  }

  /// @see net::Listener::listen().
  void listen()
  {
    listener_->listen();
  }

  /// @see net::Listener::wait().
  bool wait(std::chrono::milliseconds timeout = std::chrono::milliseconds{-1})
  {
    return listener_->wait(timeout);
  }

  /// Accepts new incoming connection..
  std::unique_ptr<Server_connection> accept();

  /// @see net::Listener::accept().
  void close()
  {
    listener_->close();
  }

private:
  std::unique_ptr<net::Listener> listener_;
  Listener_options options_;
};

inline Listener Listener_options::make_listener() const
{
  return Listener{*this};
}

std::unique_ptr<Server_connection> Listener::accept()
{
  auto io = listener_->accept();

  // Setting up timeouts.
  auto s = static_cast<net::Socket_native>(io->native_handle());
  constexpr auto timeout = 10 * 1000;
#ifdef _WIN32
  const auto rrcv = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, timeout, sizeof(DWORD));
  const auto rsnd = setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, timeout, sizeof(DWORD));
#else
  timeval timeout_{timeout / 1000, 0};
  char* const to = reinterpret_cast<char*>(&timeout_);
  const auto rrcv = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, to, sizeof(timeval));
  const auto rsnd = setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, to, sizeof(timeval));
#endif
  if (net::is_socket_error(rrcv) || net::is_socket_error(rsnd))
    throw os::Sys_exception{"setsockopt"};

  return std::unique_ptr<Server_connection>{new Server_connection{std::move(io)}};
}

} // namespace dmitigr::http

#endif  // DMITIGR_HTTP_SERVER_HPP