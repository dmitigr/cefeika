// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_CLIENT_HPP
#define DMITIGR_HTTP_CLIENT_HPP

#include "connection.hpp"
#include "types_fwd.hpp"
#include "../assert.hpp"
#include "../net/client.hpp"

namespace dmitigr::http {

/// A HTTP client connection.
class Client_connection final : public Connection {
public:
  /// The constructor.
  Client_connection(net::Client_options options)
    : options_{std::move(options)}
  {}

  /// @returns Newly created instance.
  static std::unique_ptr<Client_connection> make(net::Client_options options)
  {
    return std::make_unique<Client_connection>(std::move(options));
  }

  /// @returns The options.
  const net::Client_options& options() const
  {
    return options_;
  }

  /// @see Connection::is_server().
  bool is_server() const override
  {
    return false;
  }

  /// Connects to `remote`.
  void connect()
  {
    init(net::make_tcp_connection(options_));
  }

  /**
   * @par Requires
   * `!is_head_received()`.
   */
  void send_start(const Method method, const std::string_view path, const bool skip_headers = false)
  {
    DMITIGR_CHECK(!is_head_received());
    const auto m{to_string_view(method)};
    DMITIGR_ASSERT(!m.empty());
    std::string line;
    line.reserve(7 + 1 + path.size() + 11);
    line.append(m).append(" ").append(path).append(" HTTP/1.0\r\n");
    if (skip_headers) {
      line.append("\r\n");
      is_headers_sent_ = true;
    }
    send_start__(line);
  }

  /// Alternative of `send_start(name, value, true)`.
  void send_start_skip_headers(const Method method, const std::string_view path)
  {
    send_start(method, path, true);
  }

  /// @returns The HTTP version extracted from start line.
  std::string_view version() const override
  {
    const char* const offset = head_.data();
    return {offset, version_size_};
  }

  /// @returns The status code extracted from start line.
  std::string_view status_code() const
  {
    const char* const offset = head_.data() + version_size_ + 1;
    return {offset, code_size_};
  }

  /// @returns The status phrase extracted from start line.
  std::string_view status_phrase() const
  {
    const char* const offset = head_.data() + version_size_ + 1 + code_size_ + 1;
    return {offset, phrase_size_};
  }

private:
  net::Client_options options_;
};

} // namespace dmitigr::http

#endif  // DMITIGR_HTTP_CLIENT_HPP
