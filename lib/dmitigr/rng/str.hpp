// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rng.hpp

#ifndef DMITIGR_RNG_STR_HPP
#define DMITIGR_RNG_STR_HPP

#include "dmitigr/rng/dll.hpp"

#include <string>

namespace dmitigr::rng {

/**
 * @returns The random string of the
 * specified size from characters of `palette`.
 */
DMITIGR_RNG_API std::string random_string(const std::string& palette, std::string::size_type size);

/**
 * @returns The random string of the
 * specified size from characters in the range [beg,end).
 *
 * @par Requires
 * `(beg < end)`.
 */
DMITIGR_RNG_API std::string random_string(char beg, char end, std::string::size_type size);

} // namespace dmitigr::rng

#ifdef DMITIGR_RNG_HEADER_ONLY
#include "dmitigr/rng/str.cpp"
#endif

#endif  // DMITIGR_RNG_STR_HPP
