// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#ifndef DMITIGR_WS_TYPES_FWD_HPP
#define DMITIGR_WS_TYPES_FWD_HPP

/// The API.
namespace dmitigr::ws {

enum class Data_format;

class Connection;
class Http_io;
class Http_request;
class Listener;
class Listener_options;
class Timer;

/// The implementation details.
namespace detail {
struct Ws_data;

class iConnection;
template<bool> class Conn;

class iHttp_request;

class iHttp_io;
template<bool> class iHttp_io_templ;

class iListener;
template<bool> class Lstnr;

class iListener_options;

class iTimer;
} // namespace detail

} // namespace dmitigr::ws

#endif  // DMITIGR_WS_TYPES_FWD_HPP
