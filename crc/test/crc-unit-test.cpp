// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt

#include "../../crc.hpp"
#include "../../testo.hpp"

int main(int, char* argv[])
{
  namespace crc = dmitigr::crc;
  using namespace dmitigr::testo;

  try {
    const char* const str{"dmitigr"};
    DMITIGR_ASSERT(crc::crc16(str, 7) == 35600);
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 2;
  }
}
