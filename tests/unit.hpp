// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see file LICENSE.txt

#ifndef DMITIGR_TESTS_UNIT_HPP
#define DMITIGR_TESTS_UNIT_HPP

#include <dmitigr/util/debug.hpp>

#include <cassert>
#include <iostream>
#include <stdexcept>

#define ASSERT(a) DMITIGR_ASSERT(a)
#define DOUT(...) DMITIGR_DOUT(__VA_ARGS__)

namespace dmitigr::test {

template<typename F>
bool is_logic_throw_works(F f)
{
  bool ok{};
  try {
    f();
  } catch (const std::logic_error&) {
    ok = true;
  }
  return ok;
}

template<typename F>
bool is_runtime_throw_works(F f)
{
  bool ok{};
  try {
    f();
  } catch (const std::runtime_error&) {
    ok = true;
  }
  return ok;
}

inline void report_failure(const char* const test_name, const std::exception& e)
{
  ASSERT(test_name);
  std::cerr << "Test \"" << test_name << "\" failed (std::exception catched): " << e.what() << std::endl;
}

inline void report_failure(const char* const test_name)
{
  ASSERT(test_name);
  std::cerr << "Test \"" << test_name << "\" failed (unknown exception catched): " << std::endl;
}

} // namespace dmitigr::test

#endif // DMITIGR_TESTS_UNIT_HPP
