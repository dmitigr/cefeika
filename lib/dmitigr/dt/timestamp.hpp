// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or dt.hpp

#ifndef DMITIGR_DT_TIMESTAMP_HPP
#define DMITIGR_DT_TIMESTAMP_HPP

#include "dmitigr/dt/basics.hpp"
#include "dmitigr/dt/dll.hpp"
#include "dmitigr/dt/types_fwd.hpp"

#include <memory>

namespace dmitigr::dt {

/**
 * @brief A timestamp.
 */
class Timestamp {
public:
  /**
   * @brief The destructor.
   */
  virtual ~Timestamp() = default;

  /// @name Constructors
  /// @{

  /**
   * @brief Constructs the timestamp "1583/01/01 00:00:00".
   */
  static DMITIGR_DT_API std::unique_ptr<Timestamp> make();

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
  static DMITIGR_DT_API std::unique_ptr<Timestamp> from_rfc7231(std::string_view input);

  /**
   * @returns The copy of this instance.
   */
  virtual std::unique_ptr<Timestamp> to_timestamp() const = 0;

  /// @}

  /**
   * @returns The year.
   */
  virtual int year() const = 0;

  /**
   * @returns The month.
   */
  virtual Month month() const = 0;

  /**
   * @returns The day.
   */
  virtual int day() const = 0;

  /**
   * @returns The day of week.
   */
  virtual Day_of_week day_of_week() const = 0;

  /**
   * @returns The day of year. (Starts at 1.)
   */
  virtual int day_of_year() const = 0;

  /**
   * @returns The day of epoch (from 1583 Jan 1). (Starts at 1.)
   */
  virtual int day_of_epoch() const = 0;

  /**
   * @brief Sets the date.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `is_date_acceptable(year, month, day)`.
   */
  virtual void set_date(int year, Month month, int day) = 0;

  /**
   * @overload
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(day_of_epoch > 0)`.
   */
  virtual void set_date(int day_of_epoch) = 0;

  /**
   * @returns The hour.
   */
  virtual int hour() const = 0;

  /**
   * @brief Sets the hour.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(0 <= hour && hour <= 59)`.
   */
  virtual void set_hour(int hour) = 0;

  /**
   * @returns The minute.
   */
  virtual int minute() const = 0;

  /**
   * @brief Sets the minute.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(0 <= minute && minute <= 59)`.
   */
  virtual void set_minute(int minute) = 0;

  /**
   * @returns The second.
   */
  virtual int second() const = 0;

  /**
   * @brief Sets the second.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(0 <= second && second <= 59)`.
   */
  virtual void set_second(int second) = 0;

  /**
   * @brief Sets the time.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(0 <= hour && hour <= 59) && (0 <= minute && minute <= 59) && (0 <= second && second <= 59)`.
   */
  virtual void set_time(int hour, int minute, int second) = 0;

  /// @name Conversions
  /// @{

  /**
   * @returns The result of conversion of this instance to the instance of type
   * `std::string` according to RFC7231.
   */
  virtual std::string to_rfc7231() const = 0;

  /// @}

  /// @name Operators
  /// @{

  /**
   * @returns `true` if this instance is less than `rhs`, or `false` otherwise.
   */
  virtual bool is_less(const Timestamp* rhs) const = 0;

  /**
   * @returns `true` if this instance is equal to `rhs`, or `false` otherwise.
   */
  virtual bool is_equal(const Timestamp* rhs) const = 0;

  /// @}
private:
  friend detail::iTimestamp;

  Timestamp() = default;
};

} // namespace dmitigr::dt

#ifdef DMITIGR_DT_HEADER_ONLY
#include "dmitigr/dt/timestamp.cpp"
#endif

#endif  // DMITIGR_DT_TIMESTAMP_HPP
