// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or wsbe.hpp

#ifndef DMITIGR_WSBE_TYPES_FWD_HPP
#define DMITIGR_WSBE_TYPES_FWD_HPP

/**
 * @brief The API.
 */
namespace dmitigr::wsbe {

class Connection;
class Listener;
class Listener_options;
class Server_connection;

/**
 * @brief The implementation details.
 */
namespace detail {
class iListener;
class iListener_options;
class iServer_connection;
} // namespace detail

} // namespace dmitigr::wsbe

#endif  // DMITIGR_WSBE_TYPES_FWD_HPP
