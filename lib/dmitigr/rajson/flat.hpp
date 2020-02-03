// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rajson.hpp

#ifndef DMITIGR_RAJSON_FLAT_HPP
#define DMITIGR_RAJSON_FLAT_HPP

#include "dmitigr/rajson/dll.hpp"
#include "dmitigr/rajson/conversions.hpp"
#include "dmitigr/util/debug.hpp"

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

namespace dmitigr::rajson {

/**
 * @brief A flat JSON concept.
 */
class Flat final {
public:
  /**
   * @brief The constructor.
   */
  explicit Flat(rapidjson::Document document)
    : document_{std::move(document)}
  {
    if (document_.HasParseError())
      throw std::runtime_error{"dmitigr::rajson::Flat: JSON parse error"};
  }

  /**
   * @overload
   */
  explicit Flat(const std::string_view json)
    : Flat{to_parsed_json(json)}
  {}

  /**
   * @returns The JSON document.
   */
  const rapidjson::Document& document() const
  {
    return document_;
  }

  /**
   * @overload
   */
  rapidjson::Document& document()
  {
    return const_cast<rapidjson::Document&>(static_cast<const Flat*>(this)->document());
  }

  /**
   * @returns The node named by `name` converted to type `R` by using
   * rajson::Conversions, or `std::nullopt` if no such a node presents.
   */
  template<typename R>
  std::optional<R> optional(const std::string_view name) const
  {
    DMITIGR_REQUIRE(!name.empty(), std::invalid_argument);
    const auto& d = document();
    if (const auto m = d.FindMember(rapidjson::StringRef(name.data(), name.size())), e = d.MemberEnd(); m != e)
      return rajson::to<R>(m->value);
    else
      return std::nullopt;
  }

  /**
   * @returns The node named by `name`.
   *
   * @throws `std::runtime_error` if no such a node presents.
   */
  template<typename R>
  R mandatory(const std::string_view name) const
  {
    if (auto result = optional<R>(name))
      return *result;
    else
      throw std::runtime_error{std::string{"dmitigr::rajson::Flat: node \""}.append(name).append("\"").append(" doesn't present")};
  }

private:
  rapidjson::Document document_;
};

} // namespace dmitigr::rajson

#endif // DMITIGR_RAJSON_FLAT_HPP
