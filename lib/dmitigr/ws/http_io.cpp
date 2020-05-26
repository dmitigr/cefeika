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

  bool respond(const std::string_view data, Response_handler handler) override
  {
    DMITIGR_REQUIRE(is_valid() && !is_response_handler_set(), std::logic_error);

    if (handler) {
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
      const auto [ok, done] = send_response(data, data.size());
      (void)ok;
      return done;
    } else {
      rep_->end(data);
      rep_ = nullptr;
      DMITIGR_ASSERT(!is_valid());
      return true;
    }
  }

  std::pair<bool, bool> send_response(const std::string_view data, const int total_size) override
  {
    DMITIGR_REQUIRE(is_valid() && is_response_handler_set(), std::logic_error);
    DMITIGR_REQUIRE((total_size >= 0) &&
      (data.size() <= static_cast<decltype(data.size())>(total_size)), std::invalid_argument);

    return rep_->tryEnd(data, total_size);
  }

  bool is_response_handler_set() const override
  {
    return is_response_handler_set_;
  }

  void abort() override
  {
    DMITIGR_REQUIRE(is_valid(), std::logic_error);

    rep_->close();
  }

  void set_abort_handler(Abort_handler handler) override
  {
    DMITIGR_REQUIRE(is_valid() && !is_abort_handler_set(), std::logic_error);

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
};

} // namespace dmitigr::ws::detail