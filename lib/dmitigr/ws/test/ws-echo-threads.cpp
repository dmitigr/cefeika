// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include <dmitigr/ws.hpp>

#include <chrono>
#include <sstream>
#include <thread>

namespace ws = dmitigr::ws;

// The implementation of the thread per message approach.
// (Using a thread pool approach instead is trivial.)
class Connection : public ws::Connection {
  void handle_message(const std::string_view data, const ws::Data_format format) override
  {
    std::thread{[ws = shared_from_this(), data, format]
    {
      // Simulate a work.
      std::this_thread::sleep_for(std::chrono::seconds(3));

      // Respond by using non-blocking IO.
      ws->event_loop_call_soon([ws, data, format, tid = std::this_thread::get_id()]
      {
        if (ws->is_connected()) {
          std::ostringstream s;
          s << "Echo from thread " << tid << ": " << data;
          ws->send(s.str(), format);
        }
      });
    }}.detach();
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
  return 0;
}
