// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include "dmitigr/ws/http_io.hpp"
#include "dmitigr/ws/util.hpp"
#include "dmitigr/ws/uwebsockets.hpp"

#include <cassert>
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
    assert(rep_);
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
    assert(is_valid() && connection());
    rep_->upgrade(std::move(ws_data_), sec_ws_key_, sec_ws_protocol_, sec_ws_extensions_, ctx_);
    rep_ = nullptr;
    sec_ws_key_ = sec_ws_protocol_ = sec_ws_extensions_ = {};
    ctx_ = nullptr;
    assert(!is_valid() && !connection());
  }

  void send_status(const int code, const std::string_view phrase) override
  {
    assert(is_valid());
    rep_->writeStatus(std::to_string(code).append(" ").append(phrase));
  }

  void send_header(const std::string_view name, const std::string_view value) override
  {
    assert(is_valid());
    rep_->writeHeader(name, value);
  }

  std::pair<bool, bool> send_data(const std::string_view data, const std::uintmax_t total_size) override
  {
    assert(is_valid());
    static_assert(sizeof(decltype(data.size())) <= sizeof(decltype(total_size)));
    if (!is_response_handler_set_) {
      end(data);
      return {true, true};
    } else
      return rep_->tryEnd(data, total_size);
  }

  void end(const std::string_view data = {}) override
  {
    assert(is_valid());
    rep_->end(data);
    assert(rep_->hasResponded());
    rep_ = nullptr;
    assert(!is_valid());
  }

  void set_response_handler(Response_handler handler) override
  {
    assert(is_valid() && !is_response_handler_set());
    assert(handler);
    rep_->onWritable([this, handler = std::move(handler)](const std::uintmax_t position)
    {
      assert(rep_);
      const auto ok = handler(position);
      if (ok) {
        rep_ = nullptr;
        assert(!is_valid());
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
    assert(is_valid());
    rep_->close();
    rep_ = nullptr;
    assert(!is_valid());
  }

  void set_abort_handler(Abort_handler handler) override
  {
    assert(is_valid() && !is_abort_handler_set());
    assert(handler);
    rep_->onAborted([this, handler = std::move(handler)]
    {
      handler();
      rep_ = nullptr;
      assert(!is_valid());
    });
    is_abort_handler_set_ = true;
  }

  bool is_abort_handler_set() const override
  {
    return is_abort_handler_set_;
  }

  void set_request_handler(Request_handler handler) override
  {
    assert(is_valid() && !is_request_handler_set());
    assert(handler);
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
