// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#ifndef DMITIGR_WS_TYPES_FWD_HPP
#define DMITIGR_WS_TYPES_FWD_HPP

/**
 * @brief The API.
 */
namespace dmitigr::ws {

enum class Data_format;

class Connection;
class Http_request;
class Listener;
class Listener_options;

/**
 * @brief The implementation details.
 */
namespace detail {
class iConnection;
template<bool> class Conn;

class iHttp_request;

class iListener;
template<bool> class Lstnr;

class iListener_options;
} // namespace detail

} // namespace dmitigr::ws

#endif  // DMITIGR_WS_TYPES_FWD_HPP
