// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or process.hpp

#include "dmitigr/process/info.hpp"
#include "dmitigr/process/implementation_header.hpp"

#ifndef _WIN32
#include <unistd.h>
#endif

namespace dmitigr::process {

DMITIGR_PROCESS_INLINE Pid id()
{
#ifdef _WIN32
  return ::GetCurrentProcessId();
#else
  return ::getpid();
#endif
}

} // namespace dmitigr::process

#include "dmitigr/process/implementation_footer.hpp"
