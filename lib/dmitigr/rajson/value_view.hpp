// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rajson.hpp

#ifndef DMITIGR_RAJSON_VALUE_VIEW_HPP
#define DMITIGR_RAJSON_VALUE_VIEW_HPP

#include "dmitigr/rajson/conversions.hpp"
#include <dmitigr/base/debug.hpp>

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

namespace dmitigr::rajson {

/**
 * @brief A value view.
 */
template<class Encoding, class Allocator>
class Value_view final {
public:
  /**
   * @brief The constructor.
   */
  Value_view(const rapidjson::GenericValue<Encoding, Allocator>& value)
    : value_{value}
  {}

  /**
   * @returns The JSON value.
   */
  auto& value() const
  {
    return value_;
  }

  /**
   * @returns The iterator that points to the past of the last member.
   */
  auto end() const
  {
    return value_.MemberEnd();
  }

  /**
   * @returns The iterator that points to the member named by `name`, or `end()`
   * if no such a member presents.
   */
  auto optional_iterator(const std::string_view name) const
  {
    DMITIGR_REQUIRE(!name.empty(), std::invalid_argument);
    const auto& v = value();
    if (const auto m = v.FindMember(rapidjson::StringRef(name.data(), name.size())), e = end(); m != e)
      return m;
    else
      return e;
  }

  /**
   * @returns The iterator that points to the member named by `name`.
   *
   * @throws `std::runtime_error` if no such a member presents.
   */
  auto mandatory_iterator(const std::string_view name) const
  {
    if (auto result = optional_iterator(name); result != end())
      return result;
    else
      throw std::runtime_error{std::string{"dmitigr::rajson::Value_view: member \""}
        .append(name).append("\"").append(" doesn't present")};
  }

  /**
   * @returns The value of member named by `name` converted to type `R` by
   * using rajson::Conversions, or `std::nullopt` if no such a member presents.
   */
  template<typename R>
  std::optional<R> optional(const std::string_view name) const
  {
    if (const auto i = optional_iterator(name); i != end())
      return rajson::to<R>(i->value);
    else
      return std::nullopt;
  }

  /**
   * @returns The instance of Value_view bound to member named by `name`.
   *
   * @throws `std::runtime_error` if no such a member presents.
   */
  auto mandatory(const std::string_view name) const
  {
    return Value_view{mandatory_iterator(name)->value};
  }

  /**
   * @returns The value of member named by `name` converted to type `R` by
   * using rajson::Conversions.
   *
   * @throws `std::runtime_error` if no such a member presents.
   */
  template<typename R>
  R mandatory(const std::string_view name) const
  {
    return rajson::to<R>(mandatory_iterator(name)->value);
  }

private:
  const rapidjson::GenericValue<Encoding, Allocator>& value_;
};

} // namespace dmitigr::rajson

#endif // DMITIGR_RAJSON_VALUE_VIEW_HPP
