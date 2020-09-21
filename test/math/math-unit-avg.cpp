// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or dt.hpp

#include <dmitigr/math.hpp>
#include <dmitigr/testo.hpp>

#include <array>
#include <vector>

int main(int, char* argv[])
{
  namespace math = dmitigr::math;
  using namespace dmitigr::testo;

  try {
    constexpr auto a1 = math::avg(std::array<int, 5>{1,2,3,4,5});
    static_assert(a1 == static_cast<int>(3));
    const auto a2 = math::avg(std::vector<int>{1,2,3,4,5});
    ASSERT(a2 == static_cast<int>(3));
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 2;
  }
}
