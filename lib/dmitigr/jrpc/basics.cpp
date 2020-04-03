// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#include "dmitigr/jrpc/basics.hpp"

namespace dmitigr::jrpc {

DMITIGR_JRPC_INLINE const char* to_literal(const Parameters_notation value)
{
  switch (value) {
  case Parameters_notation::positional:
    return "positional";
  case Parameters_notation::named:
    return "named";
  }
  return nullptr;
}

} // namespace dmitigr::jrpc
