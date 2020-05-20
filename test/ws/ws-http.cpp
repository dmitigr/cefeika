// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include <dmitigr/ws.hpp>

#include <iostream>

namespace ws = dmitigr::ws;

class Connection final : public ws::Connection {
  void handle_message(const std::string_view data, const ws::Data_format format) override
  {
    send(data, format); // respond by using non-blocking IO
  }

  void handle_close(int, std::string_view) override {};
};

class Listener final : public ws::Listener {
  using ws::Listener::Listener;
  std::shared_ptr<ws::Connection> make_connection(const ws::Http_request*) const override
  {
    return std::make_shared<Connection>();
  }

  void handle_request(const ws::Http_request* const req, ws::Http_io* const io) const override
  {
    io->set_receive_handler([](auto data, bool is_completed)
    {
      std::cout << "Received data = " << data << std::endl;
      if (is_completed)
        std::cout << "Receiving complete!" << std::endl;
    });

    // io->set_ready_to_send_handler([](std::intmax_t completion_offset)
    // {
    //   std::cout << ""
    // });

    io->send_status(200, "OK");
    io->send_header("Content-Type", "text/plain");
    // io->complete("Handled HTTP response!");
    // const bool done = io->is_completed();
    // if (done)
    //   std::cout << "Handled HTTP response from " << req->remote_ip_address().to_string() << std::endl;
  }
};

int main()
{
  ws::Listener_options lo{"127.0.0.1", 9001};
  lo.set_http_enabled(true)
    .set_idle_timeout(std::chrono::seconds{120})
    .set_max_payload_size(16 * 1024);
  Listener{lo}.listen();
}
