// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or dt.hpp

#ifndef DMITIGR_DT_BASICS_HPP
#define DMITIGR_DT_BASICS_HPP

#include <dmitigr/misc/debug.hpp>

#include <string>

namespace dmitigr::dt {

/**
 * @brief A day of a week.
 */
enum class Day_of_week { sun, mon, tue, wed, thu, fri, sat };

/**
 * @brief A month.
 */
enum class Month { jan, feb, mar, apr, may, jun, jul, aug, sep, oct, nov, dec };

/**
 * @returns The days count in the specified `year`.
 *
 * @par Requires
 * `(year >= 1583)`.
 */
inline int day_count(const int year)
{
  DMITIGR_REQUIRE(year >= 1583, std::invalid_argument);

  return ((year % 100) != 0 && (year % 4) == 0) || (year % 400 == 0) ? 366 : 365;
}

/**
 * @returns `true` if the specified `year` is a leap year, or `false` otherwise.
 *
 * @par Requires
 * `(year >= 1583)`.
 */
inline bool is_leap_year(int year)
{
  return day_count(year) == 366;
}

/**
 * @returns The day count in the specified `year` and `month`.
 *
 * @par Requires
 * `(year >= 1583)`.
 */
inline int day_count(const int year, const Month month)
{
  DMITIGR_REQUIRE(year >= 1583, std::invalid_argument);

  switch (month) {
  case Month::jan: return 31;
  case Month::feb: return is_leap_year(year) ? 29 : 28;
  case Month::mar: return 31;
  case Month::apr: return 30;
  case Month::may: return 31;
  case Month::jun: return 30;
  case Month::jul: return 31;
  case Month::aug: return 31;
  case Month::sep: return 30;
  case Month::oct: return 31;
  case Month::nov: return 30;
  case Month::dec: return 31;
  };

  DMITIGR_ASSERT_ALWAYS(!true);
}

/**
 * @returns `true` if the specified date is acceptable by the API, or
 * `false` otherwise.
 *
 * @remarks Dates from 1583 Jan 1 inclusive are acceptable.
 */
inline bool is_date_acceptable(const int year, const Month month, const int day)
{
  return year >= 1583 && 1 <= day && day <= day_count(year, month);
}

/**
 * @returns The result of conversion of `str` to the value of type Day_of_week.
 *
 * @remarks The value of `str` is case-sensitive.
 */
inline Day_of_week to_day_of_week(const std::string_view str)
{
  if (str == "Mon")
    return Day_of_week::mon;
  else if (str == "Tue")
    return Day_of_week::tue;
  else if (str == "Wed")
    return Day_of_week::wed;
  else if (str == "Thu")
    return Day_of_week::thu;
  else if (str == "Fri")
    return Day_of_week::fri;
  else if (str == "Sat")
    return Day_of_week::sat;
  else if (str == "Sun")
    return Day_of_week::sun;
  else
    throw std::runtime_error{"dmitigr::dt: invalid name of day"};
}

/**
 * @returns The computed day of the week.
 *
 * @par Requires
 * `is_date_acceptable(year, month, day)`.
 */
inline Day_of_week day_of_week(const int year, const Month month, const int day)
{
  DMITIGR_REQUIRE(is_date_acceptable(year, month, day), std::invalid_argument);

  const auto month1 = static_cast<int>(month) + 1;
  const int a = (14 - month1) / 12;
  const int y = year - a;
  const int m = month1 + 12*a - 2;
  const int result = (day + y + y/4 - y/100 + y/400 + (31*m)/12) % 7;
  return static_cast<Day_of_week>(result);
}

/**
 * @returns The computed day of the year.
 *
 * @par Requires
 * `is_date_acceptable(year, month, day)`.
 *
 * @remarks Days starts at 1.
 */
inline int day_of_year(const int year, const Month month, int day)
{
  DMITIGR_REQUIRE(is_date_acceptable(year, month, day), std::invalid_argument);

  for (auto m = static_cast<int>(month) - 1; m >= 0; --m)
    day += day_count(year, static_cast<Month>(m));
  return day; // 1-based
}

/**
 * @returns The computed day of the epoch.
 *
 * @remarks Epoch starts at Jan 1 1583.
 * @remarks Days starts at 1.
 */
inline int day_of_epoch(const int year, const Month month, const int day)
{
  DMITIGR_REQUIRE(is_date_acceptable(year, month, day), std::invalid_argument);

  int result{};
  for (int y = 1583; y < year; ++y)
    result += day_count(y);
  result += day_of_year(year, month, day);
  return result; // 1-based
}

/**
 * @returns The result of conversion of `dw` to the value of type `std::string`.
 */
inline std::string to_string(const Day_of_week dw)
{
  switch (dw) {
  case Day_of_week::mon: return "Mon";
  case Day_of_week::tue: return "Tue";
  case Day_of_week::wed: return "Wed";
  case Day_of_week::thu: return "Thu";
  case Day_of_week::fri: return "Fri";
  case Day_of_week::sat: return "Sat";
  case Day_of_week::sun: return "Sun";
  }

  DMITIGR_ASSERT_ALWAYS(!true);
}

/**
 * @returns The result of conversion of `str` to the value of type Month.
 *
 * @remarks The value of `str` is case-sensitive.
 */
inline Month to_month(const std::string_view str)
{
  if (str == "Jan")
    return Month::jan;
  else if (str == "Feb")
    return Month::feb;
  else if (str == "Mar")
    return Month::mar;
  else if (str == "Apr")
    return Month::apr;
  else if (str == "May")
    return Month::may;
  else if (str == "Jun")
    return Month::jun;
  else if (str == "Jul")
    return Month::jul;
  else if (str == "Aug")
    return Month::aug;
  else if (str == "Sep")
    return Month::sep;
  else if (str == "Oct")
    return Month::oct;
  else if (str == "Nov")
    return Month::nov;
  else if (str == "Dec")
    return Month::dec;
  else
    throw std::runtime_error{"dmitigr::dt: invalid month name"};
}

/**
 * @returns The result of conversion of `month` to the value of type `std::string`.
 */
inline std::string to_string(const Month month)
{
  switch (month) {
  case Month::jan: return "Jan";
  case Month::feb: return "Feb";
  case Month::mar: return "Mar";
  case Month::apr: return "Apr";
  case Month::may: return "May";
  case Month::jun: return "Jun";
  case Month::jul: return "Jul";
  case Month::aug: return "Aug";
  case Month::sep: return "Sep";
  case Month::oct: return "Oct";
  case Month::nov: return "Nov";
  case Month::dec: return "Dec";
  }

  DMITIGR_ASSERT_ALWAYS(!true);
}

} // namespace dmitigr::dt

#endif  // DMITIGR_DT_BASICS_HPP
