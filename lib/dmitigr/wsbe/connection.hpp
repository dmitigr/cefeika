// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or wsbe.hpp

#ifndef DMITIGR_WSBE_CONNECTION_HPP
#define DMITIGR_WSBE_CONNECTION_HPP

#include "dmitigr/wsbe/basics.hpp"
#include "dmitigr/wsbe/types_fwd.hpp"

#include <string>
#include <string_view>

namespace dmitigr::wsbe {

/**
 * @brief A WebSockets connection.
 */
class Connection {
public:
  /**
   * @brief The destructor.
   */
  virtual ~Connection() = default;

  /**
   * @returns `true` if the connection is valid, or `false` otherwise.
   */
  virtual bool is_connected() const = 0;

  /**
   * @returns The textual representation of the remote IP address, or empty
   * string if this instance is not valid.
   */
  virtual std::string remote_ip_address() const = 0;

private:
  virtual void handle_open() = 0;
  virtual void handle_close() = 0;

  virtual void handle_ping() = 0;
  virtual void handle_pong() = 0;

  /**
   * @brief This function is called on every incoming message.
   */
  virtual void handle_message(std::string_view message, Opcode opcode) = 0;

  virtual void handle_drain() = 0;

private:
  friend Server_connection;

  Connection() = default;
};

} // namespace dmitigr::wsbe

#endif  // DMITIGR_WSBE_CONNECTION_HPP
