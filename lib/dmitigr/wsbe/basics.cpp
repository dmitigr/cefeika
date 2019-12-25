// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or wsbe.hpp

#include "dmitigr/wsbe/basics.hpp"
#include "dmitigr/wsbe/implementation_header.hpp"

namespace dmitigr::wsbe {

DMITIGR_WSBE_INLINE const char* to_literal(const Opcode value)
{
  switch (value) {
  case Opcode::continuation_frame:
    return "continuation_frame";
  case Opcode::text_frame:
    return "text_frame";
  case Opcode::binary_frame:
    return "binary_frame";
  case Opcode::connection_close_frame:
    return "connection_close_frame";
  case Opcode::ping_frame:
    return "ping_frame";
  case Opcode::pong_frame:
    return "pong_frame";
  }
  return nullptr;
}

} // namespace dmitigr::wsbe

#include "dmitigr/wsbe/implementation_footer.hpp"
