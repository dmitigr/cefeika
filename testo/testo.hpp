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

#ifndef DMITIGR_TESTO_TESTO_HPP
#define DMITIGR_TESTO_TESTO_HPP

#include "version.hpp"
#include "../assert.hpp"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string_view>

#ifndef ASSERT
#define ASSERT(a) DMITIGR_ASSERT(a)
#endif

namespace dmitigr::testo {

/// @returns `true` if instance of type `E` is thrown upon calling of `f`.
template<class E, typename F>
bool is_throw_works(F&& f) noexcept
{
  try {
    f();
  } catch (const E&) {
    return true;
  } catch (...) {}
  return false;
}

/// Pretty-prints `e.what()`.
inline void report_failure(const std::string_view test_name, const std::exception& e)
{
  std::cerr << "Test \"" << test_name.data() << "\" failed (std::exception catched): " << e.what() << std::endl;
}

/// @overload
inline void report_failure(const std::string_view test_name)
{
  std::cerr << "Test \"" << test_name.data() << "\" failed (unknown exception catched)" << std::endl;
}

/// @returns The duration of call of `f`.
template<typename D = std::chrono::milliseconds, typename F>
auto time(F&& f)
{
  namespace chrono = std::chrono;
  const auto start = chrono::high_resolution_clock::now();
  f();
  const auto end = chrono::high_resolution_clock::now();
  return chrono::duration_cast<D>(end - start);
}

} // namespace dmitigr::testo

#endif // DMITIGR_TESTO_TESTO_HPP
