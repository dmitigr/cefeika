// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or dt.hpp

#ifndef DMITIGR_DT_TIMESTAMP_HPP
#define DMITIGR_DT_TIMESTAMP_HPP

#include "dmitigr/dt/basics.hpp"
#include "dmitigr/dt/dll.hpp"
#include "dmitigr/dt/types_fwd.hpp"

namespace dmitigr::dt {

/**
 * @brief A timestamp.
 */
class Timestamp final {
public:
  /// @name Constructors
  /// @{

  /**
   * @brief Constructs the timestamp "1583/01/01 00:00:00".
   */
  Timestamp() = default;

  /**
   * @brief Constructs the timestamp by parsing the `input` which is compliant
   * to RFC 7231.
   *
   * Examples of valid input are:
   *
   *   1. Wed, 06 Apr 1983 17:00:00 GMT
   *
   * @param input - HTTP date.
   *
   * @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Date
   */
  static DMITIGR_DT_API Timestamp from_rfc7231(std::string_view input);

  /// @}

  /**
   * @returns The year.
   */
  DMITIGR_DT_API int year() const;

  /**
   * @returns The month.
   */
  DMITIGR_DT_API Month month() const;

  /**
   * @returns The day.
   */
  DMITIGR_DT_API int day() const;

  /**
   * @returns The day of week.
   */
  DMITIGR_DT_API Day_of_week day_of_week() const;

  /**
   * @returns The day of year. (Starts at 1.)
   */
  DMITIGR_DT_API int day_of_year() const;

  /**
   * @returns The day of epoch (from 1583 Jan 1). (Starts at 1.)
   */
  DMITIGR_DT_API int day_of_epoch() const;

  /**
   * @brief Sets the date.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `is_date_acceptable(year, month, day)`.
   */
  DMITIGR_DT_API void set_date(int year, Month month, int day);

  /**
   * @overload
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(day_of_epoch > 0)`.
   */
  DMITIGR_DT_API void set_date(int day_of_epoch);

  /**
   * @returns The hour.
   */
  DMITIGR_DT_API int hour() const;

  /**
   * @brief Sets the hour.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(0 <= hour && hour <= 59)`.
   */
  DMITIGR_DT_API void set_hour(int hour);

  /**
   * @returns The minute.
   */
  DMITIGR_DT_API int minute() const;

  /**
   * @brief Sets the minute.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(0 <= minute && minute <= 59)`.
   */
  DMITIGR_DT_API void set_minute(int minute);

  /**
   * @returns The second.
   */
  DMITIGR_DT_API int second() const;

  /**
   * @brief Sets the second.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(0 <= second && second <= 59)`.
   */
  DMITIGR_DT_API void set_second(int second);

  /**
   * @brief Sets the time.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(0 <= hour && hour <= 59) && (0 <= minute && minute <= 59) && (0 <= second && second <= 59)`.
   */
  DMITIGR_DT_API void set_time(int hour, int minute, int second);

  /// @name Conversions
  /// @{

  /**
   * @returns The result of conversion of this instance to the instance of type
   * `std::string` according to RFC7231.
   */
  DMITIGR_DT_API std::string to_rfc7231() const;

  /// @}

private:
  int day_{1};
  Month month_{Month::jan};
  int year_{1583};
  int hour_{0};
  int minute_{0};
  int second_{0};

  bool is_invariant_ok() const;
};

/**
 * @returns `true` if this instance is less than `rhs`, or `false` otherwise.
 */
DMITIGR_DT_API bool operator<(const Timestamp& lhs, const Timestamp& rhs);

/**
 * @returns `true` if this instance is equal to `rhs`, or `false` otherwise.
 */
DMITIGR_DT_API bool operator==(const Timestamp& lhs, const Timestamp& rhs);

} // namespace dmitigr::dt

#ifdef DMITIGR_DT_HEADER_ONLY
#include "dmitigr/dt/timestamp.cpp"
#endif

#endif  // DMITIGR_DT_TIMESTAMP_HPP
