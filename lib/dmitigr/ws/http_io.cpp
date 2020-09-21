// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include "dmitigr/ws/http_io.hpp"
#include "dmitigr/ws/util.hpp"
#include <dmitigr/base/debug.hpp>

#include <uwebsockets/HttpResponse.h>

#include <string>

namespace dmitigr::ws::detail {

class iHttp_io : public Http_io {
  template<bool> friend class iHttp_io_templ;

  iHttp_io() = default;
};

/// The implementation of HTTP I/O.
template<bool IsSsl>
class iHttp_io_templ final : public iHttp_io {
public:
  explicit iHttp_io_templ(uWS::HttpResponse<IsSsl>* const rep)
    : rep_{rep}
  {
    DMITIGR_ASSERT(rep_);
  }

  bool is_valid() const override
  {
    return static_cast<bool>(rep_);
  }

  Connection* connection() override
  {
    return ws_data_.conn.get();
  }

  void end_handshake() override
  {
    DMITIGR_REQUIRE(is_valid() && connection(), std::logic_error);

    rep_->upgrade(std::move(ws_data_), sec_ws_key_, sec_ws_protocol_, sec_ws_extensions_, ctx_);
    rep_ = nullptr;
    sec_ws_key_ = sec_ws_protocol_ = sec_ws_extensions_ = {};
    ctx_ = nullptr;

    DMITIGR_ASSERT(!is_valid() && !connection());
  }

  void send_status(const int code, const std::string_view phrase) override
  {
    DMITIGR_REQUIRE(is_valid(), std::logic_error);

    auto status = std::to_string(code);
    status.append(" ").append(phrase);
    rep_->writeStatus(status);
  }

  void send_header(const std::string_view name, const std::string_view value) override
  {
    DMITIGR_REQUIRE(is_valid(), std::logic_error);

    rep_->writeHeader(name, value);
  }

  std::pair<bool, bool> send_data(const std::string_view data, const int total_size) override
  {
    DMITIGR_REQUIRE(is_valid(), std::logic_error);
    DMITIGR_REQUIRE((total_size == 0) || (data.size() <= static_cast<decltype(data.size())>(total_size)),
      std::invalid_argument);

    if (!is_response_handler_set_) {
      end(data);
      return {true, true};
    } else
      return rep_->tryEnd(data, total_size);
  }

  void end(const std::string_view data = {}) override
  {
    DMITIGR_REQUIRE(is_valid(), std::logic_error);

    rep_->end(data);
    DMITIGR_ASSERT(rep_->hasResponded());
    rep_ = nullptr;

    DMITIGR_ASSERT(!is_valid());
  }

  void set_response_handler(Response_handler handler) override
  {
    DMITIGR_REQUIRE(is_valid() && !is_response_handler_set(), std::logic_error);
    DMITIGR_REQUIRE(handler, std::invalid_argument);

    rep_->onWritable([this, handler = std::move(handler)](const int position)
    {
      DMITIGR_ASSERT(rep_);
      const auto ok = handler(position);
      if (ok) {
        rep_ = nullptr;
        DMITIGR_ASSERT(!is_valid());
      }
      return ok;
    });
    is_response_handler_set_ = true;
  }

  bool is_response_handler_set() const override
  {
    return is_response_handler_set_;
  }

  void abort() override
  {
    DMITIGR_REQUIRE(is_valid(), std::logic_error);

    rep_->close();
    rep_ = nullptr;
    DMITIGR_ASSERT(!is_valid());
  }

  void set_abort_handler(Abort_handler handler) override
  {
    DMITIGR_REQUIRE(is_valid() && !is_abort_handler_set(), std::logic_error);
    DMITIGR_REQUIRE(handler, std::invalid_argument);

    rep_->onAborted([this, handler = std::move(handler)]
    {
      handler();
      rep_ = nullptr;
      DMITIGR_ASSERT(!is_valid());
    });
    is_abort_handler_set_ = true;
  }

  bool is_abort_handler_set() const override
  {
    return is_abort_handler_set_;
  }

  void set_request_handler(Request_handler handler) override
  {
    DMITIGR_REQUIRE(is_valid() && !is_request_handler_set(), std::logic_error);
    DMITIGR_REQUIRE(handler, std::invalid_argument);

    rep_->onData(std::move(handler));
    is_request_handler_set_ = true;
  }

  bool is_request_handler_set() const override
  {
    return is_request_handler_set_;
  }

private:
  template<bool> friend class detail::Lstnr;

  bool is_abort_handler_set_{};
  bool is_response_handler_set_{};
  bool is_request_handler_set_{};
  uWS::HttpResponse<IsSsl>* rep_{};
  Ws_data ws_data_;
  std::string sec_ws_key_;
  std::string sec_ws_protocol_;
  std::string sec_ws_extensions_;
  us_socket_context_t* ctx_{};
};

} // namespace dmitigr::ws::detail
