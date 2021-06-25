// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt

#include "../../que.hpp"
#include "../../testo.hpp"

#include <string_view>

int main(int, char* argv[])
{
  namespace que = dmitigr::que;
  using namespace dmitigr::testo;
  using que::Fifo_array;

  try {
    {
      Fifo_array<char, 0> a;
      DMITIGR_ASSERT(a.empty());
    }

    {
      Fifo_array<char, 7> a{'d','m','i','t','i','g','r'};
      DMITIGR_ASSERT(a.data() == std::string_view{"dmitigr"});
    }

    {
      Fifo_array<char, 128> a;

      a.push_back('D');
      DMITIGR_ASSERT(a.size() == 1);
      DMITIGR_ASSERT(a.front() == 'D');
      DMITIGR_ASSERT(a.back() == 'D');

      a.push_back('I');
      DMITIGR_ASSERT(a.size() == 2);
      DMITIGR_ASSERT(a.front() == 'D');
      DMITIGR_ASSERT(a.back() == 'I');

      a.pop_front();
      DMITIGR_ASSERT(a.size() == 1);
      DMITIGR_ASSERT(a.front() == 'I');
      DMITIGR_ASSERT(a.back() == 'I');

      a.pop_front();
      DMITIGR_ASSERT(a.size() == 0);

      a.unpop_front();
      DMITIGR_ASSERT(a.size() == 1);
      DMITIGR_ASSERT(a.front() == 'I');
      DMITIGR_ASSERT(a.back() == 'I');

      for (unsigned i = 0; i < 10; ++i) a.unpop_front();
      DMITIGR_ASSERT(a.size() == 2);
      DMITIGR_ASSERT(a.front() == 'D');
      DMITIGR_ASSERT(a.back() == 'I');

      a.pop_front();
      a.pop_front();
      DMITIGR_ASSERT(a.empty());
      for (unsigned i = 0; i < 10; ++i) a.unpop_all();
      DMITIGR_ASSERT(a.size() == 2);
      DMITIGR_ASSERT(a.front() == 'D');
      DMITIGR_ASSERT(a.back() == 'I');
    }
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 2;
  }
}
