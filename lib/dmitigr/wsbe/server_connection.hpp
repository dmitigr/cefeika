// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or wsbe.hpp

#ifndef DMITIGR_WSBE_SERVER_CONNECTION_HPP
#define DMITIGR_WSBE_SERVER_CONNECTION_HPP

#include "dmitigr/wsbe/connection.hpp"
#include "dmitigr/wsbe/dll.hpp"

#include <memory>

namespace dmitigr::wsbe {

/**
 * @brief A WebSockets server connection.
 */
class Server_connection : public Connection, public std::enable_shared_from_this<Server_connection> {
public:
  /**
   * @see Connection::is_connected().
   */
  DMITIGR_WSBE_API bool is_connected() const override;

  /**
   * @see Connection::remote_ip_address().
   */
  DMITIGR_WSBE_API std::string remote_ip_address() const override;

private:
  friend detail::iListener;

  void init(std::string remote_ip_address);

  std::unique_ptr<detail::iServer_connection> rep_;
};

} // namespace dmitigr::wsbe

#ifdef DMITIGR_WSBE_HEADER_ONLY
#include "dmitigr/wsbe/server_connection.cpp"
#endif

#endif  // DMITIGR_WSBE_SERVER_CONNECTION_HPP
