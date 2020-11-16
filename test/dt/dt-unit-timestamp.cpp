// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt

#include <dmitigr/dt.hpp>
#include <dmitigr/util/testo.hpp>

int main(int, char* argv[])
{
  namespace dt = dmitigr::dt;
  using namespace dmitigr::testo;
  using dt::Timestamp;
  using dt::Month;
  using dt::Day_of_week;

  try {
    Timestamp ts;
    ASSERT(ts.year() == 1583);
    ASSERT(ts.month() == Month::jan);
    ASSERT(ts.day() == 1);
    ASSERT(ts.hour() == 0);
    ASSERT(ts.minute() == 0);
    ASSERT(ts.second() == 0);
    ASSERT(ts.day_of_week() == Day_of_week::sat);
    ASSERT(ts.day_of_year() == 1);
    ASSERT(ts.day_of_epoch() == 1);

    ts.set_date(1);
    ASSERT(ts.year() == 1583);
    ASSERT(ts.month() == Month::jan);
    ASSERT(ts.day() == 1);

    ts.set_date(365);
    ASSERT(ts.year() == 1583);
    ASSERT(ts.month() == Month::dec);
    ASSERT(ts.day() == 31);

    ts.set_date(365 + 1);
    ASSERT(ts.year() == 1584);
    ASSERT(ts.month() == Month::jan);
    ASSERT(ts.day() == 1);

    ts.set_date(365 + 31 + 29);
    ASSERT(ts.year() == 1584);
    ASSERT(ts.month() == Month::feb);
    ASSERT(ts.day() == 29);

    ts.set_date(1988, Month::oct, 26);
    ts.set_time(19, 39, 59);
    ASSERT(ts.year() == 1988);
    ASSERT(ts.month() == Month::oct);
    ASSERT(ts.day() == 26);
    ASSERT(ts.hour() == 19);
    ASSERT(ts.minute() == 39);
    ASSERT(ts.second() == 59);
    ASSERT(ts.day_of_week() == Day_of_week::wed);
    ASSERT(ts.day_of_year() == 300);
    ASSERT(ts.day_of_epoch() == 148223);

    ts.set_date(148223);
    ASSERT(ts.year() == 1988);
    ASSERT(ts.month() == Month::oct);
    ASSERT(ts.day() == 26);
    ASSERT(ts.hour() == 19);
    ASSERT(ts.minute() == 39);
    ASSERT(ts.second() == 59);
    ASSERT(ts.day_of_week() == Day_of_week::wed);
    ASSERT(ts.day_of_year() == 300);
    ASSERT(ts.day_of_epoch() == 148223);

    {
      std::string ts_str{"Wed, 06 Apr 1983 17:00:00 GMT"};
      ts = Timestamp::from_rfc7231(ts_str);
      ASSERT(ts.day_of_week() == Day_of_week::wed);
      ASSERT(ts.day() == 6);
      ASSERT(ts.month() == Month::apr);
      ASSERT(ts.year() == 1983);
      ASSERT(ts.hour() == 17);
      ASSERT(ts.minute() == 0);
      ASSERT(ts.second() == 0);
      ASSERT(ts.day_of_week() == Day_of_week::wed);
      ASSERT(ts.day_of_year() == 96);
      ASSERT(ts.day_of_epoch() == 146193);
      ASSERT(ts.to_rfc7231() == ts_str);
    }

    {
      Timestamp ts1;
      Timestamp ts2;
      ASSERT(ts1 == ts2);

      ts2.set_hour(1);
      ASSERT(ts1 < ts2);
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
