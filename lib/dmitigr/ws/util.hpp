// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#ifndef DMITIGR_WS_UTIL_HPP
#define DMITIGR_WS_UTIL_HPP

#include "../thirdparty/usockets/libusockets_dmitigr.h"

#include <cassert>
#include <string_view>

namespace dmitigr::ws::detail {

/// The data associated with every WebSocket.
struct Ws_data final {
  std::shared_ptr<Connection> conn;
};

/// @returns Local address of the socket.
inline std::string_view local_address(const bool is_ssl, us_socket_t* const s)
{
  assert(s);
  static thread_local char buf[16];
  int ip_size = sizeof(buf);
  us_socket_local_address(is_ssl, s, buf, &ip_size);
  return std::string_view{buf, static_cast<std::string_view::size_type>(ip_size)};
}

} // namespace dmitigr::ws::detail

#endif  // DMITIGR_WS_UTIL_HPP
