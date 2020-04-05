// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rng.hpp

#ifndef DMITIGR_RNG_GEN_HPP
#define DMITIGR_RNG_GEN_HPP

#include <dmitigr/base/debug.hpp>

#include <chrono>
#include <cstdlib>

namespace dmitigr::rng {

/**
 * @brief Seeds the pseudo-random number generator.
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
constexpr T cpp_pl_3rd(const T maximum)
{
  const auto rand_num = static_cast<double>(std::rand());
  return static_cast<T>((maximum + 1) * (rand_num / RAND_MAX));
}

/**
 * @overload
 */
template<typename T>
constexpr T cpp_pl_3rd(const T minimum, const T maximum)
{
  DMITIGR_REQUIRE(minimum < maximum, std::invalid_argument);
  const auto range_length = maximum - minimum;
  return (cpp_pl_3rd(maximum) % range_length) + minimum;
}

} // namespace dmitigr::rng

#endif  // DMITIGR_RNG_GEN_HPP
