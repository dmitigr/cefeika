// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#include "dmitigr/http/date.hpp"
#include <dmitigr/base/debug.hpp>

namespace dmitigr::http::detail {

/**
 * @brief The Date implementation.
 */
class iDate final : public Date {
public:
  /**
   * @brief See Date::make();
   */
  explicit iDate(const std::string_view input)
    : iDate{dt::Timestamp::from_rfc7231(input)}
  {}

  /**
   * @brief See Date::make();
   */
  explicit iDate(std::unique_ptr<dt::Timestamp>&& ts)
    : ts_{std::move(ts)}
  {
    DMITIGR_REQUIRE(ts_, std::invalid_argument);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  iDate(const iDate& rhs)
    : ts_{rhs.ts_->to_timestamp()}
  {}

  iDate& operator=(const iDate& rhs)
  {
    iDate tmp{rhs};
    swap(tmp);
    return *this;
  }

  void swap(iDate& other)
  {
    ts_.swap(other.ts_);
  }

  // ---------------------------------------------------------------------------
  // Header overridings
  // ---------------------------------------------------------------------------

  std::unique_ptr<Header> to_header() const override
  {
    return to_date();
  }

  const std::string& field_name() const override
  {
    static const std::string result{"Date"};
    return result;
  }

  std::string to_string() const override
  {
    return ts_->to_rfc7231();
  }

  // ---------------------------------------------------------------------------
  // Date overridings
  // ---------------------------------------------------------------------------

  std::unique_ptr<Date> to_date() const override
  {
    return std::make_unique<iDate>(*this);
  }

  const dt::Timestamp* timestamp() const override
  {
    return ts_.get();
  }

  dt::Timestamp* timestamp() override
  {
    return const_cast<dt::Timestamp*>(static_cast<const iDate*>(this)->timestamp());
  }

  void set_timestamp(const dt::Timestamp* const ts) override
  {
    DMITIGR_REQUIRE(ts, std::invalid_argument);
    ts_ = ts->to_timestamp();
    DMITIGR_ASSERT(is_invariant_ok());
  }

private:
  std::unique_ptr<dt::Timestamp> ts_;

  bool is_invariant_ok() const
  {
    return true;
  }
};

} // namespace dmitigr::http::detail

namespace dmitigr::http {

DMITIGR_HTTP_INLINE std::unique_ptr<Date> Date::make(const std::string_view input)
{
  return std::make_unique<detail::iDate>(input);
}

DMITIGR_HTTP_INLINE std::unique_ptr<Date> Date::make(const dt::Timestamp* const ts)
{
  return std::make_unique<detail::iDate>(ts->to_timestamp());
}

} // namespace dmitigr::http
