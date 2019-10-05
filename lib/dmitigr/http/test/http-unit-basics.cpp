// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#include <dmitigr/http.hpp>
#include <dmitigr/util/test.hpp>

int main(int, char* argv[])
{
  namespace http = dmitigr::http;
  using namespace dmitigr::test;
  using http::Same_site;
  using http::to_same_site;
  using http::to_string;

  try {
    {
      ASSERT(to_same_site("Strict") == Same_site::strict);
      ASSERT(to_same_site("Lax") == Same_site::lax);
      ASSERT(to_string(Same_site::strict) == "Strict");
      ASSERT(to_string(Same_site::lax) == "Lax");
    }
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 2;
  }

  return 0;
}
