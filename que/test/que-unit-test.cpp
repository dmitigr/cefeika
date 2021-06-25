// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt

#include "../../que.hpp"
#include "../../testo.hpp"

int main(int, char* argv[])
{
  namespace que = dmitigr::que;
  using namespace dmitigr::testo;
  using que::Fifo_string;

  try {
    {
      Fifo_string s;
      DMITIGR_ASSERT(s.empty());
    }

    {
      Fifo_string s{"dmitigr"};
      DMITIGR_ASSERT(s.view() == "dmitigr");
    }

    {
      Fifo_string s{"dmitigr", 5};
      DMITIGR_ASSERT(s.view() == "dmiti");
    }

    {
      Fifo_string s(5, 'd');
      DMITIGR_ASSERT(s.view() == "ddddd");
    }

    {
      Fifo_string s;

      s.push_back('D');
      DMITIGR_ASSERT(s.size() == 1);
      DMITIGR_ASSERT(s.view() == "D");
      DMITIGR_ASSERT(s.front() == 'D');
      DMITIGR_ASSERT(s.back() == 'D');

      s.push_back('I');
      DMITIGR_ASSERT(s.size() == 2);
      DMITIGR_ASSERT(s.view() == "DI");
      DMITIGR_ASSERT(s.front() == 'D');
      DMITIGR_ASSERT(s.back() == 'I');

      s.pop_front();
      DMITIGR_ASSERT(s.size() == 1);
      DMITIGR_ASSERT(s.view() == "I");
      DMITIGR_ASSERT(s.front() == 'I');
      DMITIGR_ASSERT(s.back() == 'I');

      s.pop_front();
      DMITIGR_ASSERT(s.size() == 0);
      DMITIGR_ASSERT(s.view().empty());
    }
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 2;
  }
}
