// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt

#include <dmitigr/misc/assert.hpp>

#include <chrono>
#include <thread>

void handle_terminate() noexcept
{
  std::cout << "terminate handled, waiting 5 seconds";
  for (int i{}; i < 10; ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds{500});
    std::cout << "." << std::flush;;
  }
  std::cout << std::endl;
}

int main()
{
  std::set_terminate(&handle_terminate);
  dmitigr_assert(true);
  if (true)
    dmitigr_assert_always(false);
}
