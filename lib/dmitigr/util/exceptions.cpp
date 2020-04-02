// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or util.hpp

#include "dmitigr/util/exceptions.hpp"
#include "dmitigr/util/implementation_header.hpp"

#include <cassert>
#include <cstdio>

namespace dmitigr {

DMITIGR_UTIL_INLINE Sys_exception::Sys_exception(const std::string& func)
  : std::system_error{last_error(), std::system_category(), func}
{}

DMITIGR_UTIL_INLINE void Sys_exception::report(const char* const func) noexcept
{
  assert(func);
  std::fprintf(stderr, "%s(): error %d\n", func, last_error());
}

DMITIGR_UTIL_INLINE int Sys_exception::last_error() noexcept
{
#ifdef _WIN32
  return static_cast<int>(::GetLastError());
#else
  return errno;
#endif
}

} // namespace dmitigr

#include "dmitigr/util/implementation_footer.hpp"
