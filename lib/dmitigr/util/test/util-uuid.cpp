// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or util.hpp

#include <dmitigr/util.hpp>

int main(int, char* argv[])
{
  namespace math = dmitigr::math;
  namespace uuid = dmitigr::uuid;
  using namespace dmitigr::test;

  try {
    math::seed_by_now();
    const auto u = uuid::Uuid::make();
    const auto s = u->to_string();
    ASSERT(s.size() == 36);
    std::cout << s << std::endl;
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 2;
  }
  return 0;
}
