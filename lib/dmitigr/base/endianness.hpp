// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or base.hpp

#ifndef DMITIGR_BASE_ENDIANNESS_HPP
#define DMITIGR_BASE_ENDIANNESS_HPP

namespace dmitigr {

enum class Endianness {
  unknown = 0,
  big,
  little
};

inline Endianness endianness() noexcept
{
  if constexpr (sizeof(unsigned char) < sizeof(unsigned long)) {
    constexpr unsigned long number = 0x01;
    static const auto result = (reinterpret_cast<const unsigned char*>(&number)[0] == 1) ? Endianness::little : Endianness::big;
    return result;
  } else
    return Endianness::unknown;
}

} // namespace dmitigr

#endif  // DMITIGR_BASE_ENDIANNESS_HPP
