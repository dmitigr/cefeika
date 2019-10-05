// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#include <dmitigr/dt.hpp>
#include <dmitigr/http.hpp>
#include <dmitigr/util/test.hpp>

int main(int, char* argv[])
{
  namespace dt = dmitigr::dt;
  namespace http = dmitigr::http;
  using namespace dmitigr::test;
  using http::Set_cookie;
  using dt::Day_of_week;
  using dt::Month;

  try {
    {
      auto sc = Set_cookie::make("name", "value");
      ASSERT(sc->field_name() == "Set-Cookie");
    }

    {
      auto sc = Set_cookie::make("name", "value");
      ASSERT(sc->name() == "name");
      ASSERT(sc->value() == "value");
      ASSERT(!sc->expires());
      ASSERT(!sc->max_age());
      ASSERT(!sc->domain());
      ASSERT(!sc->path());
      ASSERT(!sc->is_secure());
      ASSERT(!sc->is_http_only());
    }

    {
      auto sc = Set_cookie::make("name=value");
      ASSERT(sc->name() == "name");
      ASSERT(sc->value() == "value");
      ASSERT(!sc->expires());
      ASSERT(!sc->max_age());
      ASSERT(!sc->domain());
      ASSERT(!sc->path());
      ASSERT(!sc->is_secure());
      ASSERT(!sc->is_http_only());
    }

    {
      auto sc = Set_cookie::make("name=value; Expires=Thu, 28 Feb 2019 23:59:59 GMT");
      ASSERT(sc->name() == "name");
      ASSERT(sc->value() == "value");
      const auto* const ts = sc->expires();
      ASSERT(ts);
      ASSERT(ts->day_of_week() == Day_of_week::thu);
      ASSERT(ts->day() == 28);
      ASSERT(ts->month() == Month::feb);
      ASSERT(ts->year() == 2019);
      ASSERT(ts->hour() == 23);
      ASSERT(ts->minute() == 59);
      ASSERT(ts->second() == 59);
      ASSERT(!sc->max_age());
      ASSERT(!sc->domain());
      ASSERT(!sc->path());
      ASSERT(!sc->is_secure());
      ASSERT(!sc->is_http_only());
    }

    {
      auto sc = Set_cookie::make("name=value; Max-Age=12");
      ASSERT(sc->name() == "name");
      ASSERT(sc->value() == "value");
      ASSERT(!sc->expires());
      ASSERT(sc->max_age() == 12);
      ASSERT(!sc->domain());
      ASSERT(!sc->path());
      ASSERT(!sc->is_secure());
      ASSERT(!sc->is_http_only());
    }

    {
      auto sc = Set_cookie::make("name=value; Domain=..example.com");
      ASSERT(sc->name() == "name");
      ASSERT(sc->value() == "value");
      ASSERT(!sc->expires());
      ASSERT(!sc->max_age());
      ASSERT(sc->domain() == "..example.com");
      ASSERT(!sc->path());
      ASSERT(!sc->is_secure());
      ASSERT(!sc->is_http_only());
    }

    {
      auto sc = Set_cookie::make("name=value; Path=/path/to/cool/page");
      ASSERT(sc->name() == "name");
      ASSERT(sc->value() == "value");
      ASSERT(!sc->expires());
      ASSERT(!sc->max_age());
      ASSERT(!sc->domain());
      ASSERT(sc->path() == "/path/to/cool/page");
      ASSERT(!sc->is_secure());
      ASSERT(!sc->is_http_only());
    }

    {
      auto sc = Set_cookie::make("name=value; Secure");
      ASSERT(sc->name() == "name");
      ASSERT(sc->value() == "value");
      ASSERT(!sc->expires());
      ASSERT(!sc->max_age());
      ASSERT(!sc->domain());
      ASSERT(!sc->path());
      ASSERT(sc->is_secure());
      ASSERT(!sc->is_http_only());
    }

    {
      auto sc = Set_cookie::make("name=value; HttpOnly");
      ASSERT(sc->name() == "name");
      ASSERT(sc->value() == "value");
      ASSERT(!sc->expires());
      ASSERT(!sc->max_age());
      ASSERT(!sc->domain());
      ASSERT(!sc->path());
      ASSERT(!sc->is_secure());
      ASSERT(sc->is_http_only());
    }

    {
      auto sc = Set_cookie::make("name=value; Expires=Thu, 28 Feb 2019 23:59:59 GMT; "
        "Max-Age=12; Domain=..example.com; Path=/path/to/cool/page; Secure; HttpOnly");
      ASSERT(sc->name() == "name");
      ASSERT(sc->value() == "value");
      //
      const auto* const ts = sc->expires();
      ASSERT(ts);
      ASSERT(ts->day_of_week() == Day_of_week::thu);
      ASSERT(ts->day() == 28);
      ASSERT(ts->month() == Month::feb);
      ASSERT(ts->year() == 2019);
      ASSERT(ts->hour() == 23);
      ASSERT(ts->minute() == 59);
      ASSERT(ts->second() == 59);
      //
      ASSERT(sc->max_age() == 12);
      ASSERT(sc->domain() == "..example.com");
      ASSERT(sc->path() == "/path/to/cool/page");
      ASSERT(sc->is_secure());
      ASSERT(sc->is_http_only());
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
