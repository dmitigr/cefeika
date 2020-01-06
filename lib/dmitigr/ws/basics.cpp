// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include "dmitigr/ws/basics.hpp"
#include "dmitigr/ws/implementation_header.hpp"

namespace dmitigr::ws {

DMITIGR_WS_INLINE const char* to_literal(const Data_format value)
{
  switch (value) {
  case Data_format::text:
    return "text";
  case Data_format::binary:
    return "binary";
  }
  return nullptr;
}

} // namespace dmitigr::ws

#include "dmitigr/ws/implementation_footer.hpp"
