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

#ifndef DMITIGR_MISC_DEBUG_HPP
#define DMITIGR_MISC_DEBUG_HPP

#include "dmitigr/misc/macros.hpp"

#include <cstdio>
#include <stdexcept>

namespace dmitigr {

/**
 * @brief The debug mode indicator.
 */
#ifdef NDEBUG
constexpr bool is_debug_enabled = false;
#else
constexpr bool is_debug_enabled = true;
#endif

} // namespace dmitigr

#define DMITIGR_DOUT_PRIVATE(...) do {                                  \
    std::fprintf(stderr, "Debug output from " __FILE__ ":" DMITIGR_XSTRINGIZED(__LINE__) ": " __VA_ARGS__); \
  } while (false)

#define DMITIGR_ASSERT_PRIVATE(a, t) do {                               \
    if (!(a)) {                                                         \
      DMITIGR_DOUT_PRIVATE("assertion (%s) failed\n", #a);              \
        if constexpr (t)                                                \
          throw std::logic_error{"assertion (" #a ") failed at " __FILE__ ":" DMITIGR_XSTRINGIZED(__LINE__)}; \
    }                                                                   \
  } while (false)

/**
 * @brief Prints the debug output even when `(is_debug_enabled == false)`.
 */
#define DMITIGR_DOUT_ALWAYS(...) DMITIGR_DOUT_PRIVATE(__VA_ARGS__)

/**
 * @brief Checks the assertion even when `(is_debug_enabled == false)`.
 *
 * @throws An instance of `std::logic_error` if assertion failure.
 */
#define DMITIGR_ASSERT_ALWAYS(a) DMITIGR_ASSERT_PRIVATE(a, true)

/**
 * @brief Checks the assertion even when `(is_debug_enabled == false)`.
 */
#define DMITIGR_ASSERT_NOTHROW_ALWAYS(a) DMITIGR_ASSERT_PRIVATE(a, false)

#define DMITIGR_IF_DEBUG_PRIVATE(code) do { if constexpr (dmitigr::is_debug_enabled) { code } } while (false)

/**
 * @brief Prints the debug output only when `(is_debug_enabled == true)`.
 */
#define DMITIGR_DOUT(...) DMITIGR_IF_DEBUG_PRIVATE(DMITIGR_DOUT_ALWAYS(__VA_ARGS__))

/**
 * @brief Checks the assertion only when `(is_debug_enabled == true)`.
 *
 * @throws An instance of `std::logic_error` if assertion failure.
 */
#define DMITIGR_ASSERT(a) DMITIGR_IF_DEBUG_PRIVATE(DMITIGR_ASSERT_ALWAYS(a);)

/**
 * @brief Checks the assertion only when `(is_debug_enabled == true)`.
 */
#define DMITIGR_ASSERT_NOTHROW(a) DMITIGR_IF_DEBUG_PRIVATE(DMITIGR_ASSERT_NOTHROW_ALWAYS(a);)

/**
 * @throws An instance of type `Exc` with a
 * message about an API `req` (requirement) violation.
 */
#define DMITIGR_THROW_REQUIREMENT_VIOLATED(req, Exc) {                  \
    throw Exc{"API requirement (" #req ") violated at " __FILE__ ":" DMITIGR_XSTRINGIZED(__LINE__)}; \
  }

/**
 * @brief Checks the requirement `req`.
 *
 * @throws An instance of type `Exc` if the requirement failure.
 */
#define DMITIGR_REQUIRE2(req, Exc) do {             \
    if (!(req)) {                                   \
      DMITIGR_THROW_REQUIREMENT_VIOLATED(req, Exc)  \
    }                                               \
  } while (false)

/**
 * @brief Checks the requirement `req`.
 *
 * @throws An instance of type `Exc` initialized by `msg`
 * if the requirement failure.
 */
#define DMITIGR_REQUIRE3(req, Exc, msg) do {    \
    if (!(req)) {                               \
      throw Exc{msg};                           \
    }                                           \
  } while (false)

/**
 * @brief Expands to `macro_name`.
 */
#define DMITIGR_REQUIRE_NAME_PRIVATE(_1, _2, _3, macro_name, ...) macro_name

/**
 * @brief Expands to
 *   - DMITIGR_REQUIRE2(req, Exc) if 2 arguments passed;
 *   - DMITIGR_REQUIRE3(req, Exc, msg) if 3 arguments passed.
 *
 * @remarks The dummy argument `ARG` is used to avoid the warning that ISO
 * C++11 requires at least one argument for the "..." in a variadic macro.
 */
#define DMITIGR_REQUIRE(...) \
  DMITIGR_EXPAND(DMITIGR_REQUIRE_NAME_PRIVATE(__VA_ARGS__, DMITIGR_REQUIRE3, DMITIGR_REQUIRE2, ARG)(__VA_ARGS__))

#endif  // DMITIGR_MISC_DEBUG_HPP
