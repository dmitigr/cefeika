// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or wsbe.hpp

#include "dmitigr/wsbe/server_connection.hpp"
#include "dmitigr/wsbe/implementation_header.hpp"

#include "dmitigr/util/debug.hpp"

namespace dmitigr::wsbe {

struct Server_connection::Rep final {
  Rep(const Listener* const listener, std::string remote_ip_address)
    : listener_{listener}
    , remote_ip_address_{std::move(remote_ip_address)}
  {
    DMITIGR_ASSERT(listener_);
    DMITIGR_ASSERT(7 <= remote_ip_address_.size() && remote_ip_address_.size() <= 15);
  }

  const Listener* listener_{};
  std::string remote_ip_address_;
};

DMITIGR_WSBE_INLINE Server_connection::~Server_connection() = default;

DMITIGR_WSBE_INLINE Server_connection::Server_connection() = default;

DMITIGR_WSBE_INLINE void Server_connection::init(const Listener* const listener, std::string remote_ip_address)
{
  rep_ = std::make_unique<Rep>(listener, std::move(remote_ip_address));
  DMITIGR_ASSERT(rep_);
}

DMITIGR_WSBE_INLINE const Listener* Server_connection::listener() const noexcept
{
  return rep_ ? rep_->listener_ : nullptr;
}

DMITIGR_WSBE_INLINE std::string Server_connection::remote_ip_address() const
{
  return rep_ ? rep_->remote_ip_address_ : std::string{};
}

} // namespace dmitigr::wsbe

#include "dmitigr/wsbe/implementation_footer.hpp"
