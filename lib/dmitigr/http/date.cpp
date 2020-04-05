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
    : iDate{dt::detail::iTimestamp::from_rfc7231(input)}
  {}

  /**
   * @brief See Date::make();
   */
  explicit iDate(dt::detail::iTimestamp ts)
    : ts_{std::move(ts)}
  {
    DMITIGR_ASSERT(is_invariant_ok());
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
    return ts_.to_rfc7231();
  }

  // ---------------------------------------------------------------------------
  // Date overridings
  // ---------------------------------------------------------------------------

  std::unique_ptr<Date> to_date() const override
  {
    return std::make_unique<iDate>(*this);
  }

  const dt::detail::iTimestamp* timestamp() const override
  {
    return &ts_;
  }

  dt::detail::iTimestamp* timestamp() override
  {
    return const_cast<dt::detail::iTimestamp*>(static_cast<const iDate*>(this)->timestamp());
  }

  void set_timestamp(const dt::Timestamp* const ts) override
  {
    DMITIGR_REQUIRE(ts, std::invalid_argument);

    if (const auto* const t = dynamic_cast<const dt::detail::iTimestamp*>(ts))
      ts_ = *t;
    else
      DMITIGR_ASSERT(!true);

    DMITIGR_ASSERT(is_invariant_ok());
  }

private:
  dt::detail::iTimestamp ts_;

  bool is_invariant_ok() const
  {
    return true;
  }
};

} // namespace dmitigr::http::detail

namespace dmitigr::http {

DMITIGR_HTTP_INLINE std::unique_ptr<Date> Date::make(const std::string_view input)
{
  using detail::iDate;
  return std::make_unique<iDate>(input);
}

DMITIGR_HTTP_INLINE std::unique_ptr<Date> Date::make(const dt::Timestamp* const ts)
{
  using detail::iDate;

  DMITIGR_REQUIRE(ts, std::invalid_argument);

  if (const auto* const t = dynamic_cast<const dt::detail::iTimestamp*>(ts))
    return std::make_unique<iDate>(*t);

  DMITIGR_ASSERT_ALWAYS(!true);
}

} // namespace dmitigr::http
