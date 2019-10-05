// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or url.hpp

#include <dmitigr/url.hpp>
#include <dmitigr/util/test.hpp>

int main(int, char* argv[])
{
  namespace url = dmitigr::url;
  using namespace dmitigr::test;

  try {
    {
      auto qs = url::Query_string::make("");
      ASSERT(qs->parameter_count() == 0);
      ASSERT(qs->to_string() == "");
    }

    {
      const std::string str{"param1=value1&param2=2"};
      auto qs = url::Query_string::make(str);
      ASSERT(qs->to_string() == str);
      ASSERT(qs->parameter_count() == 2);
      ASSERT(qs->has_parameter("param1"));
      ASSERT(qs->has_parameter("param2"));
      ASSERT(qs->parameter_index("param1").value() == 0);
      ASSERT(qs->parameter_index("param2").value() == 1);
      ASSERT(qs->parameter(0)->name() == "param1");
      ASSERT(qs->parameter(1)->name() == "param2");
      ASSERT(qs->parameter(0)->value() == "value1");
      ASSERT(qs->parameter("param1")->value() == "value1");
      ASSERT(qs->parameter(1)->value() == "2");
      ASSERT(qs->parameter("param2")->value() == "2");

      qs->append_parameter("param3", "3");
      ASSERT(qs->parameter_count() == 3);
      ASSERT(qs->has_parameter("param3"));
      ASSERT(qs->parameter_index("param3").value() == 2);
      ASSERT(qs->parameter(2)->name() == "param3");
      ASSERT(qs->parameter(2)->value() == "3");
      ASSERT(qs->parameter("param3")->value() == "3");

      qs->parameter(2)->set_name("p3");
      ASSERT(!qs->has_parameter("param3"));
      ASSERT(qs->has_parameter("p3"));
      ASSERT(qs->parameter_index("p3").value() == 2);
      ASSERT(qs->parameter(2)->name() == "p3");
      ASSERT(qs->parameter(2)->value() == "3");
      ASSERT(qs->parameter("p3")->value() == "3");

      qs->parameter("p3")->set_name("param3");
      ASSERT(!qs->has_parameter("p3"));
      ASSERT(qs->has_parameter("param3"));
      ASSERT(qs->parameter_index("param3").value() == 2);
      ASSERT(qs->parameter(2)->name() == "param3");
      ASSERT(qs->parameter(2)->value() == "3");
      ASSERT(qs->parameter("param3")->value() == "3");

      qs->parameter("param3")->set_value("value3");
      ASSERT(qs->parameter(2)->value() == "value3");
      ASSERT(qs->parameter("param3")->value() == "value3");

      qs->remove_parameter("param2");
      ASSERT(qs->parameter_count() == 2);
      ASSERT(!qs->has_parameter("param2"));
      ASSERT(qs->parameter_index("param2") == std::nullopt);
      ASSERT(qs->parameter(1)->name() == "param3");

      qs->remove_parameter(1);
      ASSERT(qs->parameter_count() == 1);
      ASSERT(!qs->has_parameter("param3"));
      ASSERT(qs->parameter_index("param3") == std::nullopt);
      ASSERT(qs->parameter(0)->name() == "param1");
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
