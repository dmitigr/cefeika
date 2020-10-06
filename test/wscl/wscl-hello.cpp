// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include <dmitigr/wscl.hpp>
#include <chrono>
#include <iostream>

namespace chrono = std::chrono;
namespace wscl = dmitigr::wscl;

class Connection final : public wscl::Connection {
public:
  using wscl::Connection::Connection;

private:
  void handle_open() override
  {
    assert(is_open());
    std::cout << "Connection open!" << std::endl;
    send_text("Hello from dmitigr::wscl!");
  }

  void handle_message(const std::string_view data, const bool is_binary) override
  {
    std::cout << "Received message: ";
    if (is_binary)
      std::cout << "binary data of size " << data.size();
    else
      std::cout << data;
    std::cout << std::endl;
  }

  void handle_error(const int code, const std::string_view message) override
  {
    assert(!is_open());
    std::cout << "Connection error: " << code << " (" << message << ")" << std::endl;
  }

  void handle_close(const int code, const std::string_view reason) override
  {
    assert(!is_open());
    std::cout << "Connection closed: " << code << " (" << reason << ")" << std::endl;
  }
};

void handle_signal(struct ev_loop* const loop, ev_signal* const w, const int /*revents*/)
{
  if (w->signum == SIGINT) {
    ev_break(loop, EVBREAK_ALL);
    std::clog << "Graceful shutdown by SIGINT." << std::endl;
  }
}

int main()
{
  struct ev_loop* const loop = EV_DEFAULT;
  Connection conn{loop, wscl::Connection_options{}
                  .url("ws://localhost:9001/ws")
                  .ping_interval(chrono::seconds{10})};

  ev_signal signal_watcher;
  ev_signal_init(&signal_watcher, &handle_signal, SIGINT);
  ev_signal_start(loop, &signal_watcher);
  ev_run(loop, 0);
}
