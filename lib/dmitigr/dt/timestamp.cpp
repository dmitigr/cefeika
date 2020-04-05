// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or dt.hpp

#include "dmitigr/base/debug.hpp"
#include "dmitigr/dt/timestamp.hpp"

namespace dmitigr::dt::detail {

/**
 * @brief The Timestamp implementation.
 */
class iTimestamp final : public Timestamp {
public:
  /**
   * @brief See Timestamp::make().
   */
  iTimestamp() = default;

  /**
   * @brief See Timestamp::from_rfc7231().
   */
  static iTimestamp from_rfc7231(std::string_view input)
  {
    /*
     * According to: https://tools.ietf.org/html/rfc7231#section-7.1.1.1
     */

    if (input.size() < 29)
      throw std::runtime_error{"dmitigr::dt: RFC 7231 invalid input"};

    iTimestamp result;

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

  std::unique_ptr<Timestamp> to_timestamp() const override
  {
    return std::make_unique<iTimestamp>(*this);
  }

  int year() const override
  {
    return year_;
  }

  Month month() const override
  {
    return month_;
  }

  int day() const override
  {
    return day_;
  }

  Day_of_week day_of_week() const override
  {
    return dt::day_of_week(year(), month(), day());
  }

  int day_of_year() const override
  {
    return dt::day_of_year(year(), month(), day());
  }

  int day_of_epoch() const override
  {
    return dt::day_of_epoch(year(), month(), day());
  }

  void set_date(const int year, const Month month, const int day) override
  {
    DMITIGR_REQUIRE(dt::is_date_acceptable(year, month, day), std::invalid_argument);

    year_ = year;
    month_ = month;
    day_ = day;

    DMITIGR_ASSERT(is_invariant_ok());
  }

  void set_date(int day_of_epoch) override
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

  int hour() const override
  {
    return hour_;
  }

  void set_hour(const int hour) override
  {
    DMITIGR_REQUIRE(0 <= hour && hour <= 23, std::invalid_argument);

    hour_ = hour;

    DMITIGR_ASSERT(is_invariant_ok());
  }

  int minute() const override
  {
    return minute_;
  }

  void set_minute(const int minute) override
  {
    DMITIGR_REQUIRE(0 <= minute && minute <= 59, std::invalid_argument);

    minute_ = minute;

    DMITIGR_ASSERT(is_invariant_ok());
  }

  int second() const override
  {
    return second_;
  }

  void set_second(const int second) override
  {
    DMITIGR_REQUIRE(0 <= second && second <= 59, std::invalid_argument);

    second_ = second;

    DMITIGR_ASSERT(is_invariant_ok());
  }

  void set_time(const int hour, const int minute, const int second) override
  {
    DMITIGR_REQUIRE((0 <= hour && hour <= 59) &&
      (0 <= minute && minute <= 59) && (0 <= second && second <= 59), std::invalid_argument);

    hour_ = hour;
    minute_ = minute;
    second_ = second;

    DMITIGR_ASSERT(is_invariant_ok());
  }

  std::string to_rfc7231() const override
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

  bool is_less(const Timestamp* const rhs) const override
  {
    DMITIGR_REQUIRE(rhs, std::invalid_argument);

    return year() < rhs->year() ||
      static_cast<int>(month()) < static_cast<int>(rhs->month()) ||
      day() < rhs->day() ||
      hour() < rhs->hour() ||
      minute() < rhs->minute() ||
      second() < rhs->second();
  }

  bool is_equal(const Timestamp* const rhs) const override
  {
    DMITIGR_REQUIRE(rhs, std::invalid_argument);

    return year() == rhs->year() &&
      month() == rhs->month() &&
      day() == rhs->day() &&
      hour() == rhs->hour() &&
      minute() == rhs->minute() &&
      second() == rhs->second();
  }

private:
  int day_{1};
  Month month_{Month::jan};
  int year_{1583};
  int hour_{0};
  int minute_{0};
  int second_{0};

  bool is_invariant_ok() const
  {
    const bool day_ok = (1 <= day() && day() <= day_count(year(), month()));
    const bool year_ok = (1583 <= year());
    const bool hour_ok = (0 <= hour() && hour() <= 23);
    const bool minute_ok = (0 <= minute() && minute() <= 59);
    const bool second_ok = (0 <= second() && second() <= 59);
    return day_ok && year_ok && hour_ok && minute_ok && second_ok;
  }
};

} // namespace dmitigr::dt::detail

namespace dmitigr::dt {

DMITIGR_DT_INLINE std::unique_ptr<Timestamp> Timestamp::make()
{
  using detail::iTimestamp;
  return std::make_unique<iTimestamp>();
}

DMITIGR_DT_INLINE std::unique_ptr<Timestamp> Timestamp::from_rfc7231(std::string_view input)
{
  using detail::iTimestamp;
  return std::make_unique<iTimestamp>(iTimestamp::from_rfc7231(input));
}

} // namespace dmitigr::dt
