// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#ifndef DMITIGR_JRPC_BASICS_HPP
#define DMITIGR_JRPC_BASICS_HPP

#include "dmitigr/jrpc/dll.hpp"

namespace dmitigr::jrpc {

/**
 * @brief Parameters notation.
 */
enum class Parameters_notation {
  /** Positional notation. */
  positional,

  /** Named notation. */
  named
};

/**
 * @returns The literal representation of the `value`, or `nullptr`
 * if `value` does not corresponds to any value defined by Parameters_notation.
 */
DMITIGR_JRPC_API const char* to_literal(Parameters_notation value);

} // namespace dmitigr::jrpc

#ifdef DMITIGR_JRPC_HEADER_ONLY
#include "dmitigr/jrpc/basics.cpp"
#endif

#endif  // DMITIGR_JRPC_BASICS_HPP
