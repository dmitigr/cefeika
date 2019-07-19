// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or url.hpp

#include "unit.hpp"

#include <dmitigr/util/string.hpp>
#include <dmitigr/url.hpp>

int main(int, char* argv[])
{
  namespace url = dmitigr::url;
  using namespace dmitigr::test;

  try {
    {
      using dmitigr::string::to_lowercase;
      const std::string str{"name=%D0%B4%D0%B8%D0%BC%D0%B0&%d0%b2%d0%be%d0%b7%d1%80%d0%b0%d1%81%d1%82=35"};
      auto qs = url::Query_string::make(str);
      const auto str1 = to_lowercase(str);
      const auto str2 = to_lowercase(qs->to_string());
      ASSERT(str1 == str2);
      ASSERT(qs->parameter_count() == 2);
      ASSERT(qs->has_parameter("name", 0));
      ASSERT(qs->has_parameter("возраст", 0));
      ASSERT(qs->parameter_index("name", 0).value() == 0);
      ASSERT(qs->parameter_index("возраст", 0).value() == 1);
      ASSERT(qs->parameter(0)->name() == "name");
      ASSERT(qs->parameter(1)->name() == "возраст");
      ASSERT(qs->parameter(0)->value() == "дима");
      ASSERT(qs->parameter("name")->value() == "дима");
      ASSERT(qs->parameter(1)->value() == "35");
      ASSERT(qs->parameter("возраст")->value() == "35");
    }

    {
      const std::string str{"name=%D0%B4%D0%B8%D0%BC%D0%B0%20%D0%B8%D0%B3%D1%80%D0%B8%D1%88%D0%B8%D0%BD"};
      auto qs = url::Query_string::make(str);
      ASSERT(qs->to_string() == str);
      ASSERT(qs->parameter_count() == 1);
      ASSERT(qs->has_parameter("name", 0));
      ASSERT(qs->parameter_index("name", 0).value() == 0);
      ASSERT(qs->parameter(0)->name() == "name");
      ASSERT(qs->parameter(0)->value() == "дима игришин");
    }

    {
      const std::string str_plus{"name=%D0%B4%D0%B8%D0%BC%D0%B0+%D0%B8%D0%B3%D1%80%D0%B8%D1%88%D0%B8%D0%BD"};
      const std::string str_20{"name=%D0%B4%D0%B8%D0%BC%D0%B0%20%D0%B8%D0%B3%D1%80%D0%B8%D1%88%D0%B8%D0%BD"};
      auto qs = url::Query_string::make(str_plus);
      ASSERT(qs->to_string() != str_plus); // because space is encoded as %20 rather than as '+'.
      ASSERT(qs->to_string() == str_20);
      ASSERT(qs->parameter_count() == 1);
      ASSERT(qs->has_parameter("name", 0));
      ASSERT(qs->parameter_index("name", 0).value() == 0);
      ASSERT(qs->parameter(0)->name() == "name");
      ASSERT(qs->parameter(0)->value() == "дима игришин");
    }

    {
      const std::string str{"name=%D0%B4%D0%B8%D0%BC%D0%B0%2B%D0%B8%D0%B3%D1%80%D0%B8%D1%88%D0%B8%D0%BD"};
      auto qs = url::Query_string::make(str);
      ASSERT(qs->to_string() == str);
      ASSERT(qs->parameter_count() == 1);
      ASSERT(qs->has_parameter("name", 0));
      ASSERT(qs->parameter_index("name", 0).value() == 0);
      ASSERT(qs->parameter(0)->name() == "name");
      ASSERT(qs->parameter(0)->value() == "дима+игришин");
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
