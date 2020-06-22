// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#include <dmitigr/http.hpp>
#include <dmitigr/testo.hpp>

int main(int, char* argv[])
{
  namespace chrono = std::chrono;
  namespace http = dmitigr::http;
  namespace net = dmitigr::net;
  using namespace dmitigr::testo;

  try {
    const http::Listener_options lo{"127.0.0.1", 8888, 128};
    auto l = lo.make_listener();
    l.listen();
    while (true) {
      auto conn = l.accept();
      constexpr chrono::seconds timeout{10};
      net::set_timeout(static_cast<net::Socket_native>(conn->native_handle()), timeout, timeout);
      conn->receive_head();
      if (!conn->is_head_received()) {
        conn->send_start(http::Server_errc::bad_request);
        continue;
      }

      std::string body;
      if (const auto content_length = conn->content_length()) {
        if (*content_length >= 1048576) {
          conn->send_start(http::Server_errc::payload_too_large);
          continue;
        } else
          body = conn->receive_body_to_string();
      }

      std::string response{"Start line:\n"};
      response.append("method = ").append(conn->method()).append("\n");
      response.append("path = ").append(conn->path()).append("\n");
      response.append("version = ").append(conn->version()).append("\n\n");
      if (!conn->headers().empty()) {
        response.append("Headers:\n");
        for (const auto& rh : conn->headers()) {
          response += rh.name();
          response += "<-->";
          response += rh.value();
          response += "\n";
        }
      }
      if (!body.empty()) {
        response.append("Body:\n");
        response.append(body);
      }

      conn->send_start(http::Server_succ::ok);
      conn->send_header("Server", "dmitigr");
      conn->send_header("Content-Type", "text/plain");
      //conn->send_header("Content-Disposition", "attachment; filename=a.txt");
      conn->send_header("Content-Length", std::to_string(response.size()));
      conn->send_body(response);
      //conn->send_end();
    }
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 2;
  }

  return 0;
}
