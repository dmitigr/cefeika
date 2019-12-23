// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rajson.hpp

#ifndef DMITIGR_RAJSON_CONVERSIONS_HPP
#define DMITIGR_RAJSON_CONVERSIONS_HPP

#include "dmitigr/rajson/dll.hpp"
#include "dmitigr/rajson/fwd.hpp"

#include <rapidjson/document.h>
#include <rapidjson/schema.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace dmitigr::rajson {

/**
 * @returns The result of conversion of `value` to a JSON string.
 */
template<class Encoding, class Allocator>
std::string to_stringified_json(const rapidjson::GenericValue<Encoding, Allocator>& value)
{
  /*
   * Note: The template parameter of rapidjson::Writer specified explicitly for GCC 7.
   */
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer{buffer};
  if (!value.Accept(writer))
    throw std::runtime_error{"dmitigr::rajson: to_stringified_json() accept error"};
  return std::string{buffer.GetString(), buffer.GetSize()};
}

/**
 * @returns The instance of JSON document constructed by parsing the `input`.
 */
DMITIGR_RAJSON_API rapidjson::Document to_parsed_json(const std::string_view input);

/**
 * @brief The centralized "namespace" for conversion algorithms implementations.
 */
template<typename> struct Conversions;

/**
 * @returns The result of conversion of `value` of type `Source` to the
 * value of type `Destination` by using specializations of the template
 * structure Conversions.
 */
template<typename Destination, typename Source, typename ... Types>
Destination to(Source&& value, Types&& ... args)
{
  return Conversions<Destination>::from(std::forward<Source>(value), std::forward<Types>(args)...);
}

/**
 * @brief Full specialization of Conversions for `int`.
 */
template<> struct Conversions<int> final {
  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    return value.GetInt();
  }
};

/**
 * @brief Full specialization of Conversions for `std::string`.
 */
template<>
struct Conversions<std::string> final {
  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    return std::string{value.GetString(), value.GetStringLength()};
  }
};

/**
 * @brief Full specialization of Conversions for `std::string_view`.
 */
template<>
struct Conversions<std::string_view> final {
  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    return std::string_view{value.GetString(), value.GetStringLength()};
  }
};

/**
 * @brief Partial specialization of Conversions for `rapidjson::StringRef`.
 */
template<class CharType>
struct Conversions<rapidjson::GenericStringRef<CharType>> final {
  static auto from(const std::string_view value)
  {
    return rapidjson::GenericStringRef<CharType>{value.data(), value.size()};
  }
};

/**
 * @brief Partial specialization of Conversions for `rapidjson::GenericValue`.
 */
template<class Encoding, class Allocator>
struct Conversions<rapidjson::GenericValue<Encoding, Allocator>> final {
  template<typename T>
  static auto from(T&& value, Allocator& alloc)
  {
    using U = std::decay_t<T>;
    using R = rapidjson::GenericValue<Encoding, Allocator>;

    if constexpr (std::is_arithmetic_v<U>) {
      (void)alloc;
      return R{value};
    } else if constexpr (std::is_same_v<U, std::string_view>) {
      (void)alloc;
      return R{value.data(), value.size()};
    } else
      return R{std::forward<T>(value), alloc};
  }
};

} // namespace dmitigr::rajson

#ifdef DMITIGR_RAJSON_HEADER_ONLY
#include "dmitigr/rajson/conversions.cpp"
#endif

#endif  // DMITIGR_RAJSON_CONVERSIONS_HPP
