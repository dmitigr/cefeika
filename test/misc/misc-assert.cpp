// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt

#include <dmitigr/misc/assert.hpp>
#include <dmitigr/misc/testo.hpp>

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
  namespace testo = dmitigr::testo;

  dmitigr_assert_always(testo::is_throw_works<std::logic_error>(
      []{dmitigr_check_always(false);}));

  std::set_terminate(&handle_terminate);
  dmitigr_assert(true);
  if (true)
    dmitigr_assert_always(false);
}
