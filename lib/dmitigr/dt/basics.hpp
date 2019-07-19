// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or dt.hpp

#ifndef DMITIGR_DT_BASICS_HPP
#define DMITIGR_DT_BASICS_HPP

#include "dmitigr/dt/dll.hpp"

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
 * @returns `true` if the specified date is acceptable by the API, or
 * `false` otherwise.
 *
 * @remarks Dates from 1583 Jan 1 inclusive are acceptable.
 */
DMITIGR_DT_API bool is_date_acceptable(int year, Month month, int day);

/**
 * @returns The result of conversion of `str` to the value of type Day_of_week.
 *
 * @remarks The value of `str` is case-sensitive.
 */
DMITIGR_DT_API Day_of_week to_day_of_week(std::string_view str);

/**
 * @returns The computed day of the week.
 *
 * @par Requires
 * `is_date_acceptable(year, month, day)`.
 */
DMITIGR_DT_API Day_of_week day_of_week(int year, Month month, int day);

/**
 * @returns The computed day of the year.
 *
 * @par Requires
 * `is_date_acceptable(year, month, day)`.
 *
 * @remarks Days starts at 1.
 */
DMITIGR_DT_API int day_of_year(int year, Month month, int day);

/**
 * @returns The computed day of the epoch.
 *
 * @remarks Epoch starts at Jan 1 1583.
 * @remarks Days starts at 1.
 */
DMITIGR_DT_API int day_of_epoch(int year, Month month, int day);

/**
 * @returns The result of conversion of `dw` to the value of type `std::string`.
 */
DMITIGR_DT_API std::string to_string(Day_of_week dw);

/**
 * @returns The result of conversion of `str` to the value of type Month.
 *
 * @remarks The value of `str` is case-sensitive.
 */
DMITIGR_DT_API Month to_month(std::string_view str);

/**
 * @returns The result of conversion of `month` to the value of type `std::string`.
 */
DMITIGR_DT_API std::string to_string(const Month month);

/**
 * @returns The day count in the specified `year` and `month`.
 *
 * @par Requires
 * `(year >= 1583)`.
 */
DMITIGR_DT_API int day_count(int year, Month month);

/**
 * @returns The days count in the specified `year`.
 *
 * @par Requires
 * `(year >= 1583)`.
 */
DMITIGR_DT_API int day_count(int year);

/**
 * @returns `true` if the specified `year` is a leap year, or `false` otherwise.
 *
 * @par Requires
 * `(year >= 1583)`.
 */
DMITIGR_DT_API bool is_leap_year(int year);

} // namespace dmitigr::dt

#ifdef DMITIGR_DT_HEADER_ONLY
#include "dmitigr/dt/basics.cpp"
#endif

#endif  // DMITIGR_DT_BASICS_HPP
