// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#ifndef DMITIGR_JRPC_TYPES_FWD_HPP
#define DMITIGR_JRPC_TYPES_FWD_HPP

/**
 * @brief The API.
 */
namespace dmitigr::jrpc {

enum class Server_errc;

class Response;
class Error;
class Result;

class Request;

/**
 * @brief The implementation details.
 */
namespace detail {

class iResponse;
class iError;
class iResult;

class iRequest;

} // namespace detail

} // namespace dmitigr::jrpc

#endif  // DMITIGR_JRPC_TYPES_FWD_HPP
