// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or dt.hpp

#ifndef DMITIGR_DT_TIMESTAMP_HPP
#define DMITIGR_DT_TIMESTAMP_HPP

#include "dmitigr/dt/basics.hpp"
#include <dmitigr/base/debug.hpp>

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
  static Timestamp from_rfc7231(const std::string_view input)
  {
    /*
     * According to: https://tools.ietf.org/html/rfc7231#section-7.1.1.1
     */

    if (input.size() < 29)
      throw std::runtime_error{"dmitigr::dt: RFC 7231 invalid input"};

    Timestamp result;

    static const auto process_integer = [](int& dest, const std::string& extracted, const char* const error_message)
    {
      DMITIGR_ASSERT(error_message);
      std::size_t pos{};
      dest = std::stoi(extracted, &pos);
      if (pos != extracted.size())
        throw std::runtime_error{error_message};
    };

    // Extracting the day of week.
    Day_of_week dw_extracted{}; // will be checked after extraction of day, month and year
    {
      // Note: extracted value is case-sensitive according to RFC7231.
      const std::string_view extracted{input.substr(0, 3)};
      dw_extracted = dt::to_day_of_week(extracted);
    }

    // Extracting the day.
    {
      const char* const errmsg{"dmitigr::dt: RFC 7231 invalid day"};
      const std::string extracted{input.substr(5, 2)};
      process_integer(result.day_, extracted, errmsg);
      if (result.day_ < 1 || result.day_ > 31)
        throw std::runtime_error{errmsg};
    }

    // Extracting the month.
    {
      // Note: extracted value is case-sensitive according to RFC7231.
      const std::string_view extracted{input.substr(8, 3)};
      result.month_ = dt::to_month(extracted);
    }

    // Extracting the year.
    {
      const char* const errmsg{"dmitigr::dt: RFC 7231 invalid year"};
      const std::string extracted{input.substr(12, 4)};
      process_integer(result.year_, extracted, errmsg);
      if (result.year_ < 1583)
        throw std::runtime_error{errmsg};
    }

    // Checking the day of week.
    if (const auto dw = dt::day_of_week(result.year_, result.month_, result.day_); dw != dw_extracted)
      throw std::runtime_error{"dmitigr::dt: RFC 7231 invalid day of week"};

    // Extracting the hour.
    {
      const char* const errmsg{"dmitigr::dt: RFC 7231 invalid hour"};
      const std::string extracted{input.substr(17, 2)};
      process_integer(result.hour_, extracted, errmsg);
      if (result.hour_ < 0 || result.hour_ > 23)
        throw std::runtime_error{errmsg};
    }

    // Extracting the minute.
    {
      const char* const errmsg{"dmitigr::dt: RFC 7231 invalid minute"};
      const std::string extracted{input.substr(20, 2)};
      process_integer(result.minute_, extracted, errmsg);
      if (result.minute_ < 0 || result.minute_ > 59)
        throw std::runtime_error{errmsg};
    }

    // Extracting the second.
    {
      const char* const errmsg{"dmitigr::dt: RFC 7231 invalid second"};
      const std::string extracted{input.substr(23, 2)};
      process_integer(result.second_, extracted, errmsg);
      if (result.second_ < 0 || result.second_ > 59)
        throw std::runtime_error{errmsg};
    }

    // Checking the timezone.
    {
      // Note: extracted value is case-sensitive according to RFC7231.
      if (input.substr(26, 3) != "GMT")
        throw std::runtime_error{"dmitigr::dt: RFC 7231 invalid timezone"};
    }

    DMITIGR_ASSERT(result.is_invariant_ok());

    return result;
  }

  /// @}

  /**
   * @returns The year.
   */
  int year() const
  {
    return year_;
  }

  /**
   * @returns The month.
   */
  Month month() const
  {
    return month_;
  }

  /**
   * @returns The day.
   */
  int day() const
  {
    return day_;
  }

  /**
   * @returns The day of week.
   */
  Day_of_week day_of_week() const
  {
    return dt::day_of_week(year(), month(), day());
  }

  /**
   * @returns The day of year. (Starts at 1.)
   */
  int day_of_year() const
  {
    return dt::day_of_year(year(), month(), day());
  }

  /**
   * @returns The day of epoch (from 1583 Jan 1). (Starts at 1.)
   */
  int day_of_epoch() const
  {
    return dt::day_of_epoch(year(), month(), day());
  }

  /**
   * @brief Sets the date.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `is_date_acceptable(year, month, day)`.
   */
  void set_date(const int year, const Month month, const int day)
  {
    DMITIGR_REQUIRE(dt::is_date_acceptable(year, month, day), std::invalid_argument);

    year_ = year;
    month_ = month;
    day_ = day;

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @overload
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(day_of_epoch > 0)`.
   */
  void set_date(int day_of_epoch)
  {
    DMITIGR_REQUIRE(day_of_epoch > 0, std::invalid_argument);

    int y = 1583;
    for (int dc = day_count(y); day_of_epoch > dc; dc = day_count(y)) {
      day_of_epoch -= dc;
      ++y;
    }

    DMITIGR_ASSERT(day_of_epoch <= day_count(y));

    int m = static_cast<int>(Month::jan);
    for (int dc = day_count(y, static_cast<Month>(m)); day_of_epoch > dc; dc = day_count(y, static_cast<Month>(m))) {
      day_of_epoch -= dc;
      ++m;
    }

    DMITIGR_ASSERT(static_cast<int>(Month::jan) <= m && m <= static_cast<int>(Month::dec));
    DMITIGR_ASSERT(day_of_epoch > 0);

    set_date(y, static_cast<Month>(m), day_of_epoch);
  }

  /**
   * @returns The hour.
   */
  int hour() const
  {
    return hour_;
  }

  /**
   * @brief Sets the hour.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(0 <= hour && hour <= 59)`.
   */
  void set_hour(const int hour)
  {
    DMITIGR_REQUIRE(0 <= hour && hour <= 23, std::invalid_argument);

    hour_ = hour;

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns The minute.
   */
  int minute() const
  {
    return minute_;
  }

  /**
   * @brief Sets the minute.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(0 <= minute && minute <= 59)`.
   */
  void set_minute(const int minute)
  {
    DMITIGR_REQUIRE(0 <= minute && minute <= 59, std::invalid_argument);

    minute_ = minute;

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns The second.
   */
  int second() const
  {
    return second_;
  }

  /**
   * @brief Sets the second.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(0 <= second && second <= 59)`.
   */
  void set_second(const int second)
  {
    DMITIGR_REQUIRE(0 <= second && second <= 59, std::invalid_argument);

    second_ = second;

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @brief Sets the time.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(0 <= hour && hour <= 59) && (0 <= minute && minute <= 59) && (0 <= second && second <= 59)`.
   */
  void set_time(int hour, int minute, int second)
  {
    DMITIGR_REQUIRE((0 <= hour && hour <= 59) &&
      (0 <= minute && minute <= 59) && (0 <= second && second <= 59), std::invalid_argument);

    hour_ = hour;
    minute_ = minute;
    second_ = second;

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /// @name Conversions
  /// @{

  /**
   * @returns The result of conversion of this instance to the instance of type
   * `std::string` according to RFC7231.
   */
  std::string to_rfc7231() const
  {
    static const auto to_fmt_string = [](const int num)
    {
      return (num < 10) ? std::string{"0"}.append(std::to_string(num)) : std::to_string(num);
    };

    std::string result;
    result += dt::to_string(day_of_week());
    result += ", ";
    result += to_fmt_string(day_);
    result += " ";
    result += dt::to_string(month_);
    result += " ";
    result += to_fmt_string(year_);
    result += " ";
    result += to_fmt_string(hour_);
    result += ":";
    result += to_fmt_string(minute_);
    result += ":";
    result += to_fmt_string(second_);
    result += " GMT";

    return result;
  }

  /// @}

private:
  int day_{1};
  Month month_{Month::jan};
  int year_{1583};
  int hour_{};
  int minute_{};
  int second_{};

  bool is_invariant_ok() const noexcept
  {
    const bool day_ok = (1 <= day() && day() <= day_count(year(), month()));
    const bool year_ok = (1583 <= year());
    const bool hour_ok = (0 <= hour() && hour() <= 23);
    const bool minute_ok = (0 <= minute() && minute() <= 59);
    const bool second_ok = (0 <= second() && second() <= 59);
    return day_ok && year_ok && hour_ok && minute_ok && second_ok;
  }
};

/**
 * @returns `true` if this instance is less than `rhs`, or `false` otherwise.
 */
inline bool operator<(const Timestamp& lhs, const Timestamp& rhs)
{
  return lhs.year() < rhs.year() ||
    static_cast<int>(lhs.month()) < static_cast<int>(rhs.month()) ||
    lhs.day() < rhs.day() ||
    lhs.hour() < rhs.hour() ||
    lhs.minute() < rhs.minute() ||
    lhs.second() < rhs.second();
}

/**
 * @returns `true` if this instance is less than or equal to `rhs`, or `false` otherwise.
 */
inline bool operator<=(const Timestamp& lhs, const Timestamp& rhs)
{
  return lhs.year() <= rhs.year() ||
    static_cast<int>(lhs.month()) <= static_cast<int>(rhs.month()) ||
    lhs.day() <= rhs.day() ||
    lhs.hour() <= rhs.hour() ||
    lhs.minute() <= rhs.minute() ||
    lhs.second() <= rhs.second();
}

/**
 * @returns `true` if this instance is equal to `rhs`, or `false` otherwise.
 */
inline bool operator==(const Timestamp& lhs, const Timestamp& rhs)
{
  return lhs.year() == rhs.year() &&
    lhs.month() == rhs.month() &&
    lhs.day() == rhs.day() &&
    lhs.hour() == rhs.hour() &&
    lhs.minute() == rhs.minute() &&
    lhs.second() == rhs.second();
}

/**
 * @returns `true` if this instance is not equal to `rhs`, or `false` otherwise.
 */
inline bool operator!=(const Timestamp& lhs, const Timestamp& rhs)
{
  return !(lhs == rhs);
}

/**
 * @returns `true` if this instance is greater than `rhs`, or `false` otherwise.
 */
inline bool operator>(const Timestamp& lhs, const Timestamp& rhs)
{
  return lhs.year() > rhs.year() ||
    static_cast<int>(lhs.month()) > static_cast<int>(rhs.month()) ||
    lhs.day() > rhs.day() ||
    lhs.hour() > rhs.hour() ||
    lhs.minute() > rhs.minute() ||
    lhs.second() > rhs.second();
}

/**
 * @returns `true` if this instance is greater than or equal to `rhs`, or `false` otherwise.
 */
inline bool operator>=(const Timestamp& lhs, const Timestamp& rhs)
{
  return lhs.year() >= rhs.year() ||
    static_cast<int>(lhs.month()) >= static_cast<int>(rhs.month()) ||
    lhs.day() >= rhs.day() ||
    lhs.hour() >= rhs.hour() ||
    lhs.minute() >= rhs.minute() ||
    lhs.second() >= rhs.second();
}

} // namespace dmitigr::dt

#endif  // DMITIGR_DT_TIMESTAMP_HPP
