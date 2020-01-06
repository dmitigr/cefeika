// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include <dmitigr/ws.hpp>
#include <chrono>

namespace ws = dmitigr::ws;

class Connection : public ws::Connection {
  void handle_message(const std::string_view data, const ws::Data_format format) override
  {
    send(data, format); // respond by using non-blocking IO
  }
};

class Listener : public ws::Listener {
  using ws::Listener::Listener;
  std::shared_ptr<ws::Connection> make_connection(const ws::Http_request*) const override
  {
    return std::make_shared<Connection>();
  }
};

int main()
{
  ws::Listener_options lo{"127.0.0.1", 9001};
  lo.set_idle_timeout(std::chrono::seconds{10})
    .set_max_payload_size(16 * 1024);
  Listener{lo}.listen();
}
