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

#ifndef DMITIGR_MISC_ASSERT_HPP
#define DMITIGR_MISC_ASSERT_HPP

#include <iostream>
#include <stdexcept>
#include <string>

namespace dmitigr {

/// The debug mode indicator.
#ifndef NDEBUG
constexpr bool is_debug{true};
#else
constexpr bool is_debug{false};
#endif

} // namespace dmitigr

/// Checks `a` always, even when `NDEBUG` is defined.
#define dmitigr_assert_always(a) do {                                   \
    if (!(a)) {                                                         \
      std::cerr<<"assertion ("<<#a<<") failed at "<<__FILE__<<":"<<__LINE__<<"\n"; \
      std::terminate();                                                 \
    }                                                                   \
  } while (false)

/// Checks `a` only if `NDEBUG` is not defined.
#ifndef NDEBUG
#define dmitigr_assert(a) dmitigr_assert_always(a)
#else
#define dmitigr_assert(a) ((void)0)
#endif

/// Checks `a` always, even when `NDEBUG` is defined.
#define dmitigr_check_always(a) do {                                    \
    if (!(a)) {                                                         \
      throw std::logic_error{std::string{"check ("}.append(#a)          \
          .append(") failed at ").append(__FILE__).append(":")          \
          .append(std::to_string(__LINE__))};                           \
    }                                                                   \
  } while (false)

/// Checks `a` only if `NDEBUG` is not defined.
#ifndef NDEBUG
#define dmitigr_check(a) dmitigr_check_always(a)
#else
#define dmitigr_check(a) ((void)0)
#endif

#endif  // DMITIGR_MISC_ASSERT_HPP
