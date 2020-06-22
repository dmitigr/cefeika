// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or net.hpp

#ifndef DMITIGR_NET_CLIENT_HPP
#define DMITIGR_NET_CLIENT_HPP

#include "dmitigr/net/descriptor.hpp"
#include "dmitigr/net/endpoint.hpp"
#include "dmitigr/net/socket.hpp"

#include <memory>
#include <utility>

namespace dmitigr::net {

/**
 * @returns A newly created descriptor connected over TCP (or Named Pipe)
 * to `remote` endpoint.
 */
inline std::unique_ptr<Descriptor> make_tcp_connection(const Endpoint& remote)
{
  using Sockdesc = detail::socket_Descriptor;

  static const auto make_tcp_connection = [](const Socket_address& addr)
  {
    auto result = make_tcp_socket(addr.family());
    connect_socket(result, addr);
    return result;
  };

  switch (remote.communication_mode()) {
#ifdef _WIN32
  case Communication_mode::wnp: {
    throw std::logic_error{"not implemented"};
    return nullptr;
  }
#else
  case Communication_mode::uds:
    return std::make_unique<Sockdesc>(make_tcp_connection({remote.uds_path().value()}));
#endif
  case Communication_mode::net:
    return std::make_unique<Sockdesc>(make_tcp_connection({remote.net_address().value(), remote.net_port().value()}));
  }
}

} // namespace dmitigr::net

#endif  // DMITIGR_NET_CLIENT_HPP
