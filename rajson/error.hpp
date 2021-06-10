// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rajson.hpp

#ifndef DMITIGR_RAJSON_ERROR_HPP
#define DMITIGR_RAJSON_ERROR_HPP

#include "../3rdparty/rapidjson/error/en.h"
#include "../3rdparty/rapidjson/error/error.h"

#include <system_error>

namespace std {

/**
 * @ingroup errors
 *
 * @brief The full specialization for integration with `<system_error>`.
 */
template<>
struct is_error_condition_enum<rapidjson::ParseErrorCode> final : true_type {};

} // namespace std

namespace dmitigr::rajson {

/**
 * @ingroup errors
 *
 * @brief A category of runtime client errors.
 *
 * @see Parse_exception.
 */
class Parse_error_category final : public std::error_category {
public:
  /// @returns The literal `dmitigr_rajson_parse_error`.
  const char* name() const noexcept override
  {
    return "dmitigr_rajson_parse_error";
  }

  /**
   * @returns The string that describes the error condition denoted by `ev`.
   *
   * @par Requires
   * `ev` must corresponds to the value of `rapidjson::ParseErrorCode`.
   *
   * @remarks The caller should not rely on the return value as it is a
   * subject to change.
   */
  std::string message(const int ev) const override
  {
    std::string result{name()};
    result += ' ';
    result += std::to_string(ev);
    if (const auto* const literal =
      rapidjson::GetParseError_En(static_cast<rapidjson::ParseErrorCode>(ev))) {
      result += ' ';
      result += literal;
    }
    return result;
  }
};

/**
 * @ingroup errors
 *
 * @returns The reference to the instance of type Parse_error_category.
 */
inline const Parse_error_category& parse_error_category() noexcept
{
  static const Parse_error_category result;
  return result;
}

/**
 * @ingroup errors
 *
 * @returns `std::error_condition(int(errc), parse_error_category())`
 */
inline std::error_condition
make_error_condition(const rapidjson::ParseErrorCode errc) noexcept
{
  return std::error_condition{static_cast<int>(errc), parse_error_category()};
}

/**
 * @ingroup errors
 *
 * @brief The exception thrown on parse errors.
 */
class Parse_exception : public std::runtime_error {
public:
  /// The constructor.
  explicit Parse_exception(const rapidjson::ParseResult pr)
    : runtime_error{rapidjson::GetParseError_En(pr.Code())}
    , pr_{pr}
  {}

  /// @returns The error condition.
  std::error_condition condition() const noexcept
  {
    return make_error_condition(pr_.Code());
  }

  /// @returns A parse result
  const rapidjson::ParseResult& parse_result() const noexcept
  {
    return pr_;
  }

private:
  rapidjson::ParseResult pr_;
};

} // namespace dmitigr::rajson

#endif  // DMITIGR_RAJSON_ERROR_HPP
