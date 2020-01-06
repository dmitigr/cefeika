// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include "dmitigr/ws/http_request.hpp"
#include "dmitigr/ws/implementation_header.hpp"

#include "dmitigr/util/debug.hpp"
#include "dmitigr/util/net.hpp"

#include <uwebsockets/HttpParser.h>

namespace dmitigr::ws::detail {

/**
 * @brief An implementation of HTTP request.
 */
class iHttp_request final : public Http_request {
public:
  explicit iHttp_request(uWS::HttpRequest* const rep, const std::string_view remote_ip_address_binary)
    : rep_{rep}
    , remote_ip_address_binary_{remote_ip_address_binary}
  {
    DMITIGR_ASSERT(rep_);
    DMITIGR_ASSERT(!remote_ip_address_binary_.empty());
  }

  std::string remote_ip_address() const override
  {
    const auto ip = net::Ip_address::make_from_binary(remote_ip_address_binary_);
    return ip->to_string();
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

  std::string_view header(std::string_view name) const override
  {
    return rep_->getHeader(name);
  }

private:
  uWS::HttpRequest* rep_{};
  std::string_view remote_ip_address_binary_;
};

} // namespace dmitigr::ws::detail

#include "dmitigr/ws/implementation_footer.hpp"
