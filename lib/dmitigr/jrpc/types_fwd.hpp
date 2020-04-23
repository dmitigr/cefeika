// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#ifndef DMITIGR_JRPC_TYPES_FWD_HPP
#define DMITIGR_JRPC_TYPES_FWD_HPP

/**
 * @brief The API.
 */
namespace dmitigr::jrpc {

enum class Parameters_notation;
enum class Server_errc;

class Request;
class Response;
class Error;
class Result;

/**
 * @brief The implementation details.
 */
namespace detail {
} // namespace detail

} // namespace dmitigr::jrpc

#endif  // DMITIGR_JRPC_TYPES_FWD_HPP
