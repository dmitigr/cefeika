// -*- C++ -*-
// Copyright (C) 2020 Dmitry Igrishin
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

#ifndef DMITIGR_ALGO_ALGO_HPP
#define DMITIGR_ALGO_ALGO_HPP

#include "dmitigr/algo/version.hpp"

#include <algorithm>

namespace dmitigr::algo {

/**
 * @brief Remove duplicates from the given container.
 */
template<class Container>
void eliminate_duplicates(Container& cont)
{
  std::sort(begin(cont), end(cont));
  cont.erase(std::unique(begin(cont), end(cont)), end(cont));
}

/**
 * @returns `true` if the `input` begins with the `pattern`.
 */
template<class Container>
bool is_begins_with(const Container& input, const Container& pattern)
{
  return (pattern.size() <= input.size()) &&
    std::equal(cbegin(input), cend(input), cbegin(pattern));
}

} // namespace dmitigr::algo

#endif  // DMITIGR_ALGO_ALGO_HPP
