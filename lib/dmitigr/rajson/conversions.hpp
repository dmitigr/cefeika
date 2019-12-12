// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rajson.hpp

#ifndef DMITIGR_RAJSON_CONVERSIONS_HPP
#define DMITIGR_RAJSON_CONVERSIONS_HPP

#include "dmitigr/rajson/dll.hpp"
#include "dmitigr/rajson/fwd.hpp"

#include "dmitigr/thirdparty/rapidjson/document.h"
#include "dmitigr/thirdparty/rapidjson/schema.h"
#include "dmitigr/thirdparty/rapidjson/stringbuffer.h"
#include "dmitigr/thirdparty/rapidjson/writer.h"

#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace dmitigr::json {

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
    throw std::runtime_error{"dmitigr::json::to_stringified_json(): accept error"};
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
template<typename Destination, typename Source>
Destination to(Source&& value)
{
  return Conversions<Destination>::from(std::forward<Source>(value));
}

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
 * @brief Full specialization of Conversions for `rapidjson::StringRef`.
 */
template<class CharType>
struct Conversions<rapidjson::GenericStringRef<CharType>> final {
  static auto from(const std::string_view value)
  {
    return rapidjson::GenericStringRef<CharType>{value.data(), value.size()};
  }
};

} // namespace dmitigr::json

#ifdef DMITIGR_RAJSON_HEADER_ONLY
#include "dmitigr/rajson/conversions.cpp"
#endif

#endif  // DMITIGR_RAJSON_CONVERSIONS_HPP
