// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include "dmitigr/ws/http_io.hpp"
#include <dmitigr/base/debug.hpp>

#include <uwebsockets/HttpResponse.h>

#include <string>

namespace dmitigr::ws::detail {

class iHttp_io : public Http_io {
  template<bool> friend class iHttp_io_templ;

  iHttp_io() = default;
};

/**
 * @brief An implementation of HTTP io.
 */
template<bool IsSsl>
class iHttp_io_templ final : public iHttp_io {
  template<bool> friend class Lstnr;

  explicit iHttp_io_templ(uWS::HttpResponse<IsSsl>* const rep)
    : rep_{rep}
  {
    DMITIGR_ASSERT(rep_);
  }

  void send_status(const int code, const std::string_view phrase) override
  {
    auto status = std::to_string(code);
    status.append(" ").append(phrase);
    rep_->writeStatus(status);
  }

  void send_header(const std::string_view name, const std::string_view value) override
  {
    rep_->writeHeader(name, value);
  }

  bool send_chunk(const std::string_view data) override
  {
    return rep_->write(data);
  }

  void complete(const std::string_view data) override
  {
    rep_->end(data);
  }

  std::pair<bool, bool> try_complete(std::string_view data, std::intmax_t total_size) override
  {
    return rep_->tryEnd(data, static_cast<int>(total_size));
  }

  std::intmax_t completion_offset() override
  {
    return rep_->getWriteOffset();
  }

  bool is_completed() const override
  {
    return rep_->hasResponded();
  }

  void abort() override
  {
    rep_->close();
  }

  void set_receive_handler(std::function<void(std::string_view, bool)> handler) override
  {
    rep_->onData(std::move(handler));
  }

  void set_ready_to_send_handler(std::function<bool(std::intmax_t)> handler) override
  {
    rep_->onWritable(std::move(handler));
  }

  void set_abort_handler(std::function<void()> handler) override
  {
    rep_->onAborted(std::move(handler));
  }

  uWS::HttpResponse<IsSsl>* rep_{};
};

} // namespace dmitigr::ws::detail
