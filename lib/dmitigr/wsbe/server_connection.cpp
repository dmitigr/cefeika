// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or wsbe.hpp

#include "dmitigr/wsbe/server_connection.hpp"
#include "dmitigr/wsbe/implementation_header.hpp"

#include "dmitigr/util/debug.hpp"

namespace dmitigr::wsbe::detail {

class iServer_connection final {
public:
  explicit iServer_connection(std::string remote_ip_address)
    : remote_ip_address_{std::move(remote_ip_address)}
  {
    DMITIGR_ASSERT(7 <= remote_ip_address_.size() && remote_ip_address_.size() <= 15);
  }

  std::string remote_ip_address_;
};

} // namespace dmitigr::wsbe::detail

namespace dmitigr::wsbe {

DMITIGR_WSBE_INLINE void Server_connection::init(std::string remote_ip_address)
{
  rep_ = std::make_unique<detail::iServer_connection>(std::move(remote_ip_address));
  DMITIGR_ASSERT(rep_);
}

DMITIGR_WSBE_INLINE bool Server_connection::is_connected() const
{
  return static_cast<bool>(rep_);
}

DMITIGR_WSBE_INLINE std::string Server_connection::remote_ip_address() const
{
  return rep_ ? rep_->remote_ip_address_ : std::string{};
}

} // namespace dmitigr::wsbe

#include "dmitigr/wsbe/implementation_footer.hpp"
