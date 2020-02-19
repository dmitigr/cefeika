// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or process.hpp

#ifndef DMITIGR_PROCESS_INFO_HPP
#define DMITIGR_PROCESS_INFO_HPP

#include "dmitigr/process/dll.hpp"

#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
  #endif
  #include <Windows.h>
#else
  #include <sys/types.h>
#endif

namespace dmitigr::process {

#ifdef _WIN32
/// The alias of the process identifier type.
using Pid = DWORD;
#else
/// The alias of the process identifier type.
using Pid = ::pid_t;
#endif

/// @returns The current process identifier of the calling process.
DMITIGR_PROCESS_API Pid id();

} // namespace dmitigr::process

#ifdef DMITIGR_PROCESS_HEADER_ONLY
#include "dmitigr/process/info.cpp"
#endif

#endif  // DMITIGR_PROCESS_INFO_HPP
