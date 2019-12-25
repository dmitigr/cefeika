// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or wsbe.hpp

#ifndef DMITIGR_WSBE_SERVER_CONNECTION_HPP
#define DMITIGR_WSBE_SERVER_CONNECTION_HPP

#include "dmitigr/wsbe/basics.hpp"
#include "dmitigr/wsbe/dll.hpp"
#include "dmitigr/wsbe/types_fwd.hpp"

#include <memory>
#include <string>
#include <string_view>

namespace dmitigr::wsbe {

/**
 * @brief A WebSockets server connection.
 */
class Server_connection : public std::enable_shared_from_this<Server_connection> {
public:
  /**
   * @brief The destructor.
   */
  virtual DMITIGR_WSBE_API ~Server_connection();

  /**
   * @returns The listener of this instance, or `nullptr` if this instance
   * is not valid.
   */
  DMITIGR_WSBE_API const Listener* listener() const noexcept;

  /**
   * @returns The textual representation of the remote IP address, or empty
   * string if this instance is not valid.
   */
  DMITIGR_WSBE_API std::string remote_ip_address() const;

protected:
  /**
   * @brief The constructor.
   */
  DMITIGR_WSBE_API Server_connection();

private:
  /**
   * @brief This function is called by Server on every incoming message.
   *
   * @remarks Throwing exceptions is safe, since the Server can automatically
   * handle them.
   */
  virtual void handle(std::string_view message, Opcode opcode) = 0;

private:
  friend detail::iListener;

  void init(const Listener* const listener, std::string remote_ip_address);

  struct Rep;
  std::unique_ptr<Rep> rep_;
};

} // namespace dmitigr::wsbe

#ifdef DMITIGR_WSBE_HEADER_ONLY
#include "dmitigr/wsbe/server_connection.cpp"
#endif

#endif  // DMITIGR_WSBE_SERVER_CONNECTION_HPP
