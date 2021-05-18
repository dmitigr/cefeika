// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt

#include "../../ws.hpp"

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>

namespace ws = dmitigr::ws;

std::atomic_bool is_running;

void handle_signal(const int)
{
  is_running = false;
}

class Connection final : public ws::Connection {
  void handle_message(const std::string_view data, const ws::Data_format format) override
  {
    send(data, format); // respond by using non-blocking IO
  }

  void handle_close(int, std::string_view) override {}
  void handle_drain() override {}
};

class Listener final : public ws::Listener {
  using ws::Listener::Listener;
  std::shared_ptr<ws::Connection> handle_handshake(const ws::Http_request&, std::shared_ptr<ws::Http_io>) override
  {
    return std::make_shared<Connection>();
  }
};

int main()
{
  using namespace std::chrono;
  std::signal(SIGINT,  &handle_signal);
  std::signal(SIGTERM, &handle_signal);
  ws::Listener_options lo{"0.0.0.0", 9001};
  lo.set_idle_timeout(std::chrono::seconds{10})
    .set_max_payload_size(16 * 1024);
  Listener l{lo};
  l.add_timer("closer").set_handler([&l]{
    std::cout << "Handle of timer closer invoked, is_running = " << is_running << std::endl;
    if (!is_running) {
      l.remove_timer("closer");
      l.close();
    }
  }).start(milliseconds{1000}, seconds{3});
  is_running = true;
  l.listen();
}
