// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or util.hpp

#ifndef DMITIGR_UTIL_MATH_HPP
#define DMITIGR_UTIL_MATH_HPP

#include "dmitigr/util/debug.hpp"

#include <chrono>
#include <cstdlib>

namespace dmitigr::math {

/**
 * Seeds the pseudo-random number generator.
 */
inline void seed_by_now()
{
  const auto seed = std::chrono::duration_cast<std::chrono::seconds>(
    std::chrono::system_clock::now().time_since_epoch()).count();
  std::srand(static_cast<unsigned>(seed));
}

/**
 * @returns The random number.
 *
 * @remarks From TC++PL 3rd, 22.7.
 */
template<typename T>
constexpr T rand_cpp_pl_3rd(const T maximum)
{
  const auto rand_num = static_cast<double>(std::rand());
  return static_cast<T>((maximum + 1) * (rand_num / RAND_MAX));
}

/**
 * @overload
 */
template<typename T>
constexpr T rand_cpp_pl_3rd(const T minimum, const T maximum)
{
  DMITIGR_REQUIRE(minimum < maximum, std::invalid_argument);
  const auto range_length = maximum - minimum;
  return (rand_cpp_pl_3rd(maximum) % range_length) + minimum;
}

/**
 * @returns `true` if `number` is a power of 2, or `false` otherwise.
 */
template<typename T>
constexpr bool is_power_of_two(const T number)
{
  return (number & (number - 1)) == 0;
}

/**
 * @returns The number to add to `size` to
 * get the aligned value by using `alignment`.
 *
 * @par Requires
 * `is_power_of_two(alignment)`.
 */
template<typename T, typename U>
constexpr T padding(const T size, const U alignment)
{
  DMITIGR_REQUIRE(is_power_of_two(alignment), std::invalid_argument);
  const T a = alignment;
  return (static_cast<T>(0) - size) & (a - 1);
}

/**
 * @return The value of `size` aligned by using `alignment`.
 *
 * @par Requires
 * `is_power_of_two(alignment)`.
 */
template<typename T, typename U>
constexpr T aligned(const T size, const U alignment)
{
  DMITIGR_REQUIRE(is_power_of_two(alignment), std::invalid_argument);
  const T a = alignment;
  return (size + (a - 1)) & -a;
}

} // namespace dmitigr::math

#endif  // DMITIGR_UTIL_MATH_HPP
