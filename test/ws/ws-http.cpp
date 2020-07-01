// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include <dmitigr/ws.hpp>

#include <cstdint>
#include <iostream>

namespace ws = dmitigr::ws;

class Connection final : public ws::Connection {
  void handle_message(const std::string_view data, const ws::Data_format format) override
  {
    send(data, format); // respond by using non-blocking IO
  }

  void handle_close(int, std::string_view) override {};
  void handle_drain() override {};
};

class Listener final : public ws::Listener {
  using ws::Listener::Listener;
  std::shared_ptr<ws::Connection> make_connection() override
  {
    return std::make_shared<Connection>();
  }

  void handle_request(const ws::Http_request&, std::shared_ptr<ws::Http_io> io) override
  {
    io->set_request_handler([total_size = std::intmax_t{}](auto data, bool is_last) mutable
    {
      total_size += data.size();
      std::cout << "Received portion of data, size =  " << data.size() << std::endl;
      if (is_last)
        std::cout << "Received the last portion of data! Total data size = " << total_size << std::endl;
    });

    io->set_abort_handler([]
    {
      std::cout << "Invoked abort handler" << std::endl;
    });

    io->send_status(200, "OK");
    io->send_header("Content-Type", "text/plain");
    io->send_header("Content-Disposition", "filename=ws-http-test-data.txt");
    const auto data = std::make_shared<std::string>(32'000'000, 'a');
    io->set_response_handler([io, data](const int pos) -> bool
    {
      std::cout << "Ready to send handler invoked. Current data position = " << pos << std::endl;
      std::string_view dv{data->data() + pos, data->size() - pos};
      const auto [ok, done] = io->send_data(dv, data->size());
      (void)done;
      return ok;
    });
    io->send_data(*data);
  }
};

int main()
{
  ws::Listener_options lo{"0.0.0.0", 9001};
  lo.set_http_enabled(true)
    .set_idle_timeout(std::chrono::seconds{120})
    .set_max_payload_size(16 * 1024);
  Listener{lo}.listen();
}
