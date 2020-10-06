# Asynchronous WebSocket client in C++

Dmitigr Wscl (hereinafter referred to as Wscl) - is an asynchronous
WebSocket client in C++. (It's just wraps modified [libuwsc] at the moment.)

## Features

- asynchronous (by using [libev] at the moment);
- easy to use;
- supports SSL.

## Third-party dependencies

- [CMake] build system version 3.13+;
- C++17 compiler (such as [GCC]);
- [libev] high-performance event loop;
- [OpenSSL] (optionally) SSL implementation.

## Third-party software bundled

|Name|Source|
|:---|:------|
|buffer|https://github.com/dmitigr/buffer|
|libuwsc|https://github.com/dmitigr/libuwsc|

## Documentation

The [Doxygen]-generated documentation will be available soon.

## Hello, World

```cpp
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
    std::cout << "Connection error: " << code << " (" << message << ")" << std::endl;
  }

  void handle_close(const int code, const std::string_view reason) override
  {
    std::cout << "Connection closed: " << code << " (" << reason << ")" << std::endl;
  }
};

void handle_signal(struct ev_loop* const loop, ev_signal* const sig, const int /*revents*/) noexcept
{
  if (sig->signum == SIGINT) {
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
```

[CMake]: https://cmake.org/
[Doxygen]: http://doxygen.org/
[GCC]: https://gcc.gnu.org/
[buffer]: https://github.com/dmitigr/buffer
[libev]: http://software.schmorp.de/pkg/libev.html
[libuwsc]: https://github.com/dmitigr/libuwsc
[OpenSSL]: https://www.openssl.org/
