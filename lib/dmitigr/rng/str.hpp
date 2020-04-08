// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rng.hpp

#ifndef DMITIGR_RNG_STR_HPP
#define DMITIGR_RNG_STR_HPP

#include <dmitigr/rng/gen.hpp>

#include <string>

namespace dmitigr::rng {

/**
 * @returns The random string.
 *
 * @param size The result size.
 * @param palette The palette of characters the result will consist of.
 */
inline std::string random_string(const std::string& palette,
  const std::string::size_type size)
{
  std::string result;
  result.resize(size);
  if (const auto pallete_size = palette.size()) {
    using Counter = std::remove_const_t<decltype (pallete_size)>;
    for (Counter i = 0; i < size; ++i)
      result[i] = palette[rng::cpp_pl_3rd(pallete_size)];
  }
  return result;
}

/**
 * @returns The random string.
 *
 * @param size The result size.
 * @param beg The start of source range.
 * @param end The past of end of source range.
 *
 * @par Requires
 * `(beg <= end)`.
 */
inline std::string random_string(const char beg, const char end,
  const std::string::size_type size)
{
  DMITIGR_REQUIRE(beg <= end, std::out_of_range);
  std::string result;
  if (beg < end) {
    result.resize(size);
    const auto length = end - beg;
    using Counter = std::remove_const_t<decltype (size)>;
    for (Counter i = 0; i < size; ++i)
      result[i] = static_cast<char>((rng::cpp_pl_3rd(end) % length) + beg);
  }
  return result;
}

} // namespace dmitigr::rng

#endif  // DMITIGR_RNG_STR_HPP
