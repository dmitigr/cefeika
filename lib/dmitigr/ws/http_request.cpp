// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include "dmitigr/ws/http_request.hpp"
#include <dmitigr/base/debug.hpp>

#include <uwebsockets/HttpParser.h>

namespace dmitigr::ws::detail {

/**
 * @brief An implementation of HTTP request.
 */
class iHttp_request final : public Http_request {
public:
  explicit iHttp_request(uWS::HttpRequest* const rep,
    const std::string_view remote_ip_address_binary,
    const std::string_view local_ip_address_binary)
    : rep_{rep}
    , remote_ip_{net::Ip_address::from_binary(remote_ip_address_binary)}
    , local_ip_{net::Ip_address::from_binary(local_ip_address_binary)}
  {
    DMITIGR_ASSERT(rep_);
  }

  const net::Ip_address& remote_ip_address() const override
  {
    return remote_ip_;
  }

  const net::Ip_address& local_ip_address() const override
  {
    return local_ip_;
  }

  std::string_view method() const override
  {
    return rep_->getMethod();
  }

  std::string_view path() const override
  {
    return rep_->getUrl();
  }

  std::string_view query_string() const override
  {
    return rep_->getQuery();
  }

  std::string_view header(const std::string_view name) const override
  {
    return rep_->getHeader(name);
  }

private:
  uWS::HttpRequest* rep_{};
  net::Ip_address remote_ip_;
  net::Ip_address local_ip_;
};

} // namespace dmitigr::ws::detail
