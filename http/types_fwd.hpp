// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_TYPES_FWD_HPP
#define DMITIGR_HTTP_TYPES_FWD_HPP

/**
 * @brief The API.
 */
namespace dmitigr::http {

enum class Same_site;

/**
 * @defgroup headers Headers
 */

class Header;
class Cookie_entry;
class Cookie;
class Date;
class Set_cookie;

class Connection;
class Listener_options;
class Listener;
class Server_connection;

/**
 * @brief The implementation details.
 */
namespace detail {
} // namespace detail

} // namespace dmitigr::http

#endif  // DMITIGR_HTTP_TYPES_FWD_HPP
