// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#ifndef DMITIGR_PGFE_MESSAGE_HPP
#define DMITIGR_PGFE_MESSAGE_HPP

#include "dmitigr/pgfe/types_fwd.hpp"

namespace dmitigr::pgfe {

/**
 * @ingroup main
 *
 * @brief A client/server message to/from a PostgreSQL server.
 */
class Message {
public:
  /// The destructor.
  virtual ~Message() = default;

private:
  friend Server_message;

  Message() = default;
};

} // namespace dmitigr::pgfe

#endif  // DMITIGR_PGFE_MESSAGE_HPP
