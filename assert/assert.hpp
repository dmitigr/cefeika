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

#ifndef DMITIGR_ASSERT_ASSERT_HPP
#define DMITIGR_ASSERT_ASSERT_HPP

#include <iostream>
#include <stdexcept>
#include <type_traits>

namespace dmitigr {

/// The debug mode indicator.
#ifndef NDEBUG
constexpr bool is_debug{true};
#else
constexpr bool is_debug{false};
#endif

/// Logic error - exception class derived from `std::logic_error`.
template<class E>
class Exception : public E {
public:
  /// Lifted constructors.
  using E::E;

  /// The constructor.
  Exception(const char* const file, const int line, const char* const what)
    : E{what}
    , file_{file}
    , line_{line}
  {
    static_assert(std::is_base_of_v<std::exception, Exception>);
  }

  /// @returns The name of file from where the exception thrown.
  const char* file() const noexcept
  {
    return file_;
  }

  /// @returns The line of file from where the exception thrown.
  int line() const noexcept
  {
    return line_;
  }

private:
  const char* file_{};
  int line_{};
};

} // namespace dmitigr

// Helpers
#define DMITIGR_ASSERT_STR(s) #s
#define DMITIGR_ASSERT_XSTR(s) DMITIGR_ASSERT_STR(s)

/// Checks `a` always, regardless of `NDEBUG`.
#define DMITIGR_ASSERT(a) do {                                          \
    if (!(a)) {                                                         \
      std::cerr<<"assertion ("<<#a<<") failed at "<<__FILE__<<":"<<__LINE__<<"\n"; \
      std::terminate();                                                 \
    }                                                                   \
  } while (false)

/// Checks `a` always, regardless of `NDEBUG`.
#define DMITIGR_CHECK_GENERIC(a, E) do {                                \
    if (!(a)) {                                                         \
      throw dmitigr::Exception<E>{__FILE__, __LINE__,                   \
        "check (" #a ") failed at " __FILE__ ":" DMITIGR_ASSERT_XSTR(__LINE__)}; \
    }                                                                   \
  } while (false)

#define DMITIGR_CHECK(a) DMITIGR_CHECK_GENERIC(a, std::logic_error)
#define DMITIGR_CHECK_ARG(a) DMITIGR_CHECK_GENERIC(a, std::invalid_argument)
#define DMITIGR_CHECK_DOMAIN(a) DMITIGR_CHECK_GENERIC(a, std::domain_error)
#define DMITIGR_CHECK_LENGTH(a) DMITIGR_CHECK_GENERIC(a, std::length_error)
#define DMITIGR_CHECK_RANGE(a) DMITIGR_CHECK_GENERIC(a, std::out_of_range)

#endif  // DMITIGR_ASSERT_ASSERT_HPP
