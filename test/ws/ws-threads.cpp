// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt

#include <dmitigr/misc/rng.hpp>
#include <dmitigr/misc/testo.hpp>
#include <dmitigr/ws.hpp>

#include <chrono>
#include <sstream>
#include <thread>
#include <vector>

namespace rng = dmitigr::rng;
namespace ws = dmitigr::ws;

namespace {

class Connection : public ws::Connection {
  /**
   * @brief Attempts to reply to the message from several threads. Emulates
   * the situation when the some random thread closes the connection.
   */
  void handle_message(const std::string_view data, const ws::Data_format format) override
  {
    std::clog << "The message with data \"" << data << "\" is handled" << std::endl;
    std::vector<std::thread> workers{16};
    const auto closer_index = rng::cpp_pl_3rd(workers.size());
    for (std::size_t i = 0; i < workers.size(); ++i) {
      workers[i] = std::thread{[ws = shared_from_this(), format, is_closer = (i == closer_index)]
      {
        if (!is_closer) {
          ws->event_loop_call_soon([ws, format, tid = std::this_thread::get_id()]
          {
            if (ws->is_connected()) {
              std::ostringstream s;
              s << "Hello from thread " << tid;
              ws->send(s.str(), format);
            } else
              std::clog << "ignoring the message from thread " << tid << " (connection closed)" << std::endl;
          });
        } else
          ws->event_loop_call_soon([ws]
          {
            ws->close(0, "closed (expected)");
          });
      }};
    }
    for (auto& w : workers)
      w.join();
  }

  void handle_close(const int /*code*/, const std::string_view /*reason*/) override
  {
    std::string ip = is_connected() ? remote_ip_address() : "unknown IP";
    std::clog << "The connection to " << ip << " is about to close" << std::endl;
  }

  void handle_drain() override {}
};

class Listener : public ws::Listener {
  using ws::Listener::Listener;

  std::shared_ptr<ws::Connection> handle_handshake(const ws::Http_request&, std::shared_ptr<ws::Http_io>) override
  {
    std::clog << "The connection is about to be opened" << std::endl;
    const bool is_should_be_created = rng::cpp_pl_3rd(1);
    return is_should_be_created ? std::make_shared<Connection>() : nullptr;
  }
};

} // namespace

int main(int, char* argv[])
{
  using namespace dmitigr::testo;

  rng::seed_by_now();

  try {
    constexpr auto listening_duration = std::chrono::seconds{15};
    Listener listener{[]
    {
      ws::Listener_options lo{"127.0.0.1", 9001};
      lo
        .set_idle_timeout(std::chrono::milliseconds{4000})
        .set_max_payload_size(16 * 1024);
      std::clog << "The idle timeout is set to " <<
        std::chrono::duration_cast<std::chrono::seconds>(lo.idle_timeout().value()).count() << " seconds " << std::endl
                << "The max payload size is set to " <<
        lo.max_payload_size() << std::endl;
      return lo;
    }()};
    std::thread listener_thread{[&listener, listening_duration]
    {
      std::clog << "Starting WebSocket listener. Listening socket will be closed in "
                << listening_duration.count() << " seconds." << std::endl;
      listener.listen();
    }};
    std::thread finalizer_thread{[&listener, listening_duration]
    {
      std::this_thread::sleep_for(listening_duration);
      listener.close();
      std::clog << "The WebSocket listener is closed." << std::endl;
    }};
    finalizer_thread.join();
    listener_thread.join();
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 2;
  }
}
