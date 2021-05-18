// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or uuid.hpp

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This file is generated automatically. Edit version.hpp.in instead!!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef DMITIGR_UUID_VERSION_HPP
#define DMITIGR_UUID_VERSION_HPP

#include <cstdint>

namespace dmitigr::uuid {

/// @returns The library version.
constexpr std::int_fast32_t version() noexcept
{
  // Actual values are set in CMakeLists.txt.
  constexpr std::int_least32_t major = 0;
  constexpr std::int_least32_t minor = 1;

  // 11.234 -> 11 * 1000 + 234 = 11234
  return major*1000 + minor;
}

} // namespace dmitigr::uuid

#endif  // DMITIGR_UUID_VERSION_HPP
