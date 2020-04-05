// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or testo.hpp

#ifndef DMITIGR_TESTO_TESTO_HPP
#define DMITIGR_TESTO_TESTO_HPP

#include <dmitigr/base/debug.hpp>

#include <iostream>
#include <stdexcept>
#include <string_view>

#define ASSERT(a) DMITIGR_ASSERT(a)
#define DOUT(...) DMITIGR_DOUT(__VA_ARGS__)

namespace dmitigr::testo {

template<typename F>
bool is_logic_throw_works(F&& f) noexcept
{
  try {
    f();
  } catch (const std::logic_error&) {
    return true;
  } catch (...) {}
  return false;
}

template<typename F>
bool is_runtime_throw_works(F&& f) noexcept
{
  try {
    f();
  } catch (const std::runtime_error&) {
    return true;
  } catch (...) {}
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

} // namespace dmitigr::testo

#endif // DMITIGR_TESTO_TESTO_HPP
