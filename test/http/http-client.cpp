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
    auto conn = http::Client_connection::make({"127.0.0.1", 8888});
    ASSERT(!conn->is_server());
    conn->connect();
    conn->send_start(http::Method::get, "/");
    conn->send_end();
    conn->receive_head();
    if (!conn->is_head_received())
      throw std::runtime_error{"could not receive head"};

    std::string body;
    if (const auto content_length = conn->content_length()) {
      if (*content_length >= 1048576)
        throw std::runtime_error{"payload too large"};
      else
        body = conn->receive_body_to_string();
    }

    std::string response{"Start line:\n"};
    response.append("version = ").append(conn->version()).append("\n");
    response.append("status code = ").append(conn->status_code()).append("\n");
    response.append("status phrase = ").append(conn->status_phrase()).append("\n");
    response.append("Headers:\n");
    for (const auto& rh : conn->headers()) {
      response += rh.name();
      response += "<-->";
      response += rh.value();
      response += "\n";
    }
    if (!body.empty()) {
      response.append("Body:\n");
      response.append(body);
    }
    std::cout << response;
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 2;
  }

  return 0;
}
