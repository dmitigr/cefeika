// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#ifndef DMITIGR_WS_BASICS_HPP
#define DMITIGR_WS_BASICS_HPP

#include "dmitigr/ws/dll.hpp"

namespace dmitigr::ws {

/**
 * @brief A possible data format.
 */
enum class Data_format {
  /** The text format. */
  text = 0,

  /** The binary format. */
  binary = 1
};

/**
 * @returns The literal representation of the `value`, or `nullptr`
 * if `value` does not corresponds to any value defined by enum.
 */
DMITIGR_WS_API const char* to_literal(Data_format value);

} // namespace dmitigr::ws

#ifdef DMITIGR_WS_HEADER_ONLY
#include "dmitigr/ws/basics.cpp"
#endif

#endif  // DMITIGR_WS_BASICS_HPP
