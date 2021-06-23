// -*- C++ -*-
// Copyright (C) 2021 Dmitry Igrishin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
// Dmitry Igrishin
// dmitigr@gmail.com

#ifndef DMITIGR_CRC_CRC_HPP
#define DMITIGR_CRC_CRC_HPP

#include "version.hpp"

#include <cstdint>

namespace dmitigr::crc {

/**
 * @returns CRC-16 calculated from the given data, or `0` if `(!data || !size)`.
 *
 * @tparam kPoly Polynomial.
 * @param data The data for which CRC-16 has to be computed.
 * @param size The size of `data` in bytes.
 */
template<std::uint16_t kPoly = 0x8005>
constexpr std::uint16_t crc16(const char* const data,
  const std::size_t size) noexcept
{
  static_assert(kPoly > 1<<15, "Polynomial of CRC-16 must be 17 bits of length");
  if (!data || !size) return 0;

  std::uint16_t result{};
  bool is_high_bit_on{};

  // Process the words except the last one.
  std::uint8_t byte = data[0];
  for (std::size_t pos{}, bits_processed{}; pos < size;) {
    is_high_bit_on = result >> 15;
    result <<= 1;
    result |= (byte >> bits_processed) & 1;
    if (is_high_bit_on) result ^= kPoly;

    ++bits_processed;
    if (bits_processed == 8) {
      ++pos;
      bits_processed = 0;
      byte = data[pos];
    }
  }
  // Process the last word.
  for (unsigned bits_processed{}; bits_processed < 16; ++bits_processed) {
    is_high_bit_on = result >> 15;
    result <<= 1;
    if (is_high_bit_on) result ^= kPoly;
  }

  // Reverse the result.
  result = (result & 0x5555) << 1 | (result & 0xAAAA) >> 1;
  result = (result & 0x3333) << 2 | (result & 0xCCCC) >> 2;
  result = (result & 0x0F0F) << 4 | (result & 0xF0F0) >> 4;
  result = (result & 0x00FF) << 8 | (result & 0xFF00) >> 8;
  return result;
}
static_assert(!crc16(nullptr, 0));
static_assert(!crc16("dmitigr", 0));
static_assert(crc16("dmitigr", 7) == 35600);

} // namespace dmitigr::crc

#endif  // DMITIGR_CRC_CRC_HPP
