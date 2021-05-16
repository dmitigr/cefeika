// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rajson.hpp

#ifndef DMITIGR_RAJSON_VALUE_VIEW_HPP
#define DMITIGR_RAJSON_VALUE_VIEW_HPP

#include "conversions.hpp"

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>

namespace dmitigr::rajson {

/// A value view.
template<class GenericValue>
class Value_view final {
public:
  /// An alias of underlying type.
  using Underlying_type = GenericValue;

  /// The constructor.
  Value_view(GenericValue& value)
    : value_{value}
  {}

  /// @returns The JSON value.
  const auto& value() const
  {
    return value_;
  }

  /// @overload
  auto& value()
  {
    return value_;
  }

  /**
   * @returns The iterator that points to the member named by `name`, or
   * `value().MemberEnd()` if no such a member presents.
   */
  auto optional_iterator(const std::string_view name) const
  {
    return optional_iterator__(value_, name);
  }

  /// @overload
  auto optional_iterator(const std::string_view name)
  {
    return optional_iterator__(value_, name);
  }

  /**
   * @returns The iterator that points to the member named by `name`.
   *
   * @throws `std::runtime_error` if no such a member presents.
   */
  auto mandatory_iterator(const std::string_view name) const
  {
    return mandatory_iterator__(*this, name);
  }

  /// @overload
  auto mandatory_iterator(const std::string_view name)
  {
    return mandatory_iterator__(*this, name);
  }

  /**
   * @returns The value of member named by `name` converted to type `R` by
   * using rajson::Conversions, or `std::nullopt` if no such a member presents.
   */
  template<typename R>
  std::optional<R> optional(const std::string_view name) const
  {
    return optional__<R>(*this, name);
  }

  /// @overload
  template<typename R>
  std::optional<R> optional(const std::string_view name)
  {
    return optional__<R>(*this, name);
  }

  /**
   * @returns The instance of `std::optional<Value_view>` bound to member
   * named by `name`.
   */
  auto optional(const std::string_view name) const
  {
    return optional__(*this, name);
  }

  /// @overload
  auto optional(const std::string_view name)
  {
    return optional__(*this, name);
  }

  /**
   * @returns The instance of Value_view bound to member named by `name`.
   *
   * @throws `std::runtime_error` if no such a member presents.
   */
  auto mandatory(const std::string_view name) const
  {
    return mandatory__(*this, name);
  }

  /// @overload
  auto mandatory(const std::string_view name)
  {
    return mandatory__(*this, name);
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
  Underlying_type& value_;

  template<class Value>
  static auto optional_iterator__(Value& value, const std::string_view name)
  {
    if (const auto e = value.MemberEnd(); name.empty())
      return e;
    else if (const auto m = value.FindMember(rapidjson::StringRef(name.data(), name.size())); m != e)
      return m;
    else
      return e;
  }

  template<class ValueView>
  static auto mandatory_iterator__(ValueView& view, const std::string_view name)
  {
    if (auto result = view.optional_iterator(name); result != view.value_.MemberEnd())
      return result;
    else
      throw std::runtime_error{std::string{"dmitigr::rajson::Value_view: member \""}
        .append(name).append("\"").append(" doesn't present")};
  }

  template<typename R, class ValueView>
  static std::optional<R> optional__(ValueView& view, const std::string_view name)
  {
    if (const auto i = view.optional_iterator(name); i != view.value_.MemberEnd())
      return rajson::to<R>(i->value);
    else
      return std::nullopt;
  }

  template<class ValueView>
  static auto optional__(ValueView& view, const std::string_view name)
  {
    using Value = decltype(view.optional_iterator(name)->value);
    using Result = std::conditional_t<
      std::is_const_v<typename std::remove_reference_t<decltype(view.value_)>>,
      std::optional<Value_view<std::add_const_t<Value>>>,
      std::optional<Value_view<Value>>
      >;
    if (const auto i = view.optional_iterator(name); i != view.value_.MemberEnd())
      return Result{i->value};
    else
      return Result{};
  }

  template<class ValueView>
  static auto mandatory__(ValueView& view, const std::string_view name)
  {
    using Value = decltype(view.mandatory_iterator(name)->value);
    using Result = std::conditional_t<
      std::is_const_v<typename std::remove_reference_t<decltype(view.value_)>>,
      Value_view<std::add_const_t<Value>>,
      Value_view<Value>>;
    return Result{view.mandatory_iterator(name)->value};
  }
};

} // namespace dmitigr::rajson

#endif // DMITIGR_RAJSON_VALUE_VIEW_HPP
