// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#include <dmitigr/dt.hpp>
#include <dmitigr/http.hpp>
#include <dmitigr/testo.hpp>

int main(int, char* argv[])
{
  namespace dt = dmitigr::dt;
  namespace http = dmitigr::http;
  using namespace dmitigr::testo;
  using http::Date;
  using dt::Day_of_week;
  using dt::Month;
  using dt::Timestamp;

  try {
    {
      const auto d = Date::make("Sat, 06 Apr 2019 17:01:02 GMT");
      ASSERT(d->field_name() == "Date");
      const auto& ts = d->timestamp();
      ASSERT(ts.day_of_week() == Day_of_week::sat);
      ASSERT(ts.day() == 6);
      ASSERT(ts.month() == Month::apr);
      ASSERT(ts.year() == 2019);
      ASSERT(ts.hour() == 17);
      ASSERT(ts.minute() == 1);
      ASSERT(ts.second() == 2);
      //
      const auto d_copy = d->to_date();
      ASSERT(d->timestamp() == d_copy->timestamp());
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
