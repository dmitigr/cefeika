// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or base.hpp

#ifndef DMITIGR_BASE_TEST_HPP
#define DMITIGR_BASE_TEST_HPP

#include "dmitigr/base/debug.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

#define ASSERT(a) DMITIGR_ASSERT(a)
#define DOUT(...) DMITIGR_DOUT(__VA_ARGS__)

namespace dmitigr::test {

template<typename F>
bool is_logic_throw_works(F f)
{
  try {
    f();
  } catch (const std::logic_error&) {
    return true;
  }
  return false;
}

template<typename F>
bool is_runtime_throw_works(F f)
{
  try {
    f();
  } catch (const std::runtime_error&) {
    return true;
  }
  return false;
}

inline void report_failure(const std::string_view test_name, const std::exception& e)
{
  std::cerr << "Test \"" << test_name.data() << "\" failed (std::exception catched): " << e.what() << std::endl;
}

inline void report_failure(const std::string_view test_name)
{
  std::cerr << "Test \"" << test_name.data() << "\" failed (unknown exception catched)" << std::endl;
}

} // namespace dmitigr::test

#endif // DMITIGR_BASE_TEST_HPP
