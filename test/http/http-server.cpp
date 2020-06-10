// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#include <dmitigr/http.hpp>
#include <dmitigr/testo.hpp>

int main(int, char* argv[])
{
  namespace http = dmitigr::http;
  using namespace dmitigr::testo;

  try {
    const http::Listener_options lo{"127.0.0.1", 8888, 128};
    auto l = lo.make_listener();
    l.listen();
    while (true) {
      auto conn = l.accept();
      conn->receive_head();
      if (!conn->is_head_received()) {
        std::cerr << "bad request" << std::endl;
        continue;
      }

      std::cout << "method = " << conn->method() << std::endl;
      std::cout << "path = " << conn->path() << std::endl;
      std::cout << "version = " << conn->version() << std::endl;

      for (const auto& rh : conn->headers())
        std::cout << rh.name() << "<-->" << rh.value() << std::endl;

      unsigned len{};
      if (const auto cl = conn->header("content-length"); !cl.empty())
        len = std::stoi(std::string{cl});

      std::cerr << "content-length = " << len << std::endl;

      unsigned offset{};
      std::string body;
      body.resize(4096);
      while (const auto n = conn->receive_body(body.data() + offset, 4096)) {
        if (n < 4096) {
          body.resize(body.size() - 4096 + n);
          break;
        }
        offset += 4096;
        body.resize(body.size() + 4096);
      }

      std::cout << "body:" << std::endl
                << body << std::endl;

      conn->send_start(http::Server_succ::ok);
      conn->send_header("Server", "dmitigr");
      // conn.send_header("Content-Type", "text/plain");
      // conn.send_header("Content-Length", std::to_string(body.size()));
      // conn.send_body(body);

      //std::cerr << body << std::endl;
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
