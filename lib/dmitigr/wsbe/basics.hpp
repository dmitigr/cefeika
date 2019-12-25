// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or wsbe.hpp

#ifndef DMITIGR_WSBE_BASICS_HPP
#define DMITIGR_WSBE_BASICS_HPP

#include "dmitigr/wsbe/dll.hpp"

namespace dmitigr::wsbe {

/**
 * @brief WebSockets Opcodes.
 */
enum class Opcode {
  continuation_frame = 0,
  text_frame = 1,
  binary_frame = 2,
  connection_close_frame = 8,
  ping_frame = 9,
  pong_frame = 10
};

/**
 * @returns The literal representation of the `value`, or `nullptr`
 * if `value` does not corresponds to any value defined by Opcode.
 */
DMITIGR_WSBE_API const char* to_literal(Opcode value);

} // namespace dmitigr::wsbe

#ifdef DMITIGR_WSBE_HEADER_ONLY
#include "dmitigr/wsbe/basics.cpp"
#endif

#endif  // DMITIGR_WSBE_BASICS_HPP
