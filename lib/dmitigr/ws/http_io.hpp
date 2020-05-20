// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#ifndef DMITIGR_WS_HTTP_IO_HPP
#define DMITIGR_WS_HTTP_IO_HPP

#include "dmitigr/ws/types_fwd.hpp"

#include <cstdint>
#include <functional>
#include <string_view>
#include <utility>

namespace dmitigr::ws {

/**
 * @brief A HTTP I/O.
 */
class Http_io {
public:
  /**
   * @brief The destructor.
   */
  virtual ~Http_io() = default;

  virtual void send_status(int code, std::string_view phrase) = 0;

  virtual void send_header(std::string_view name, std::string_view value) = 0;

  virtual bool send_chunk(std::string_view data) = 0;

  virtual void complete(std::string_view data = {}) = 0;

  virtual std::pair<bool, bool> try_complete(std::string_view data, std::intmax_t total_size) = 0;

  virtual std::intmax_t completion_offset() = 0;

  virtual bool is_completed() const = 0;

  virtual void abort() = 0;

  virtual void set_receive_handler(std::function<void(std::string_view data,
      bool is_completed)> handler) = 0;

  /**
   * handler - is a function which accepts a value returned by completion_offset()
   * and returns `true` if write operation was successful or `false` otherwise.
   * If write operation was never performed the handler must return `true`.
   */
  virtual void set_ready_to_send_handler(std::function<bool(std::intmax_t completion_offset)> handler) = 0;

  virtual void set_abort_handler(std::function<void()> handler) = 0;

private:
  friend detail::iHttp_io;

  Http_io() = default;
};

} // namespace dmitigr::ws

#ifdef DMITIGR_WS_HEADER_ONLY
#include "dmitigr/ws/http_io.cpp"
#endif

#endif  // DMITIGR_WS_HTTP_IO_HPP
