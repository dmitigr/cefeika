// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rajson.hpp

#ifndef DMITIGR_RAJSON_CONVERSIONS_HPP
#define DMITIGR_RAJSON_CONVERSIONS_HPP

#include "dmitigr/rajson/fwd.hpp"
#include "../thirdparty/rapidjson/document.h"
#include "../thirdparty/rapidjson/schema.h"
#include "../thirdparty/rapidjson/stringbuffer.h"
#include "../thirdparty/rapidjson/writer.h"

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace dmitigr::rajson {

/// @returns The result of conversion of `value` to a JSON string.
template<class Encoding, class Allocator>
std::string to_stringified(const rapidjson::GenericValue<Encoding, Allocator>& value)
{
  rapidjson::StringBuffer buf;
  rapidjson::Writer<decltype(buf)> writer{buf}; // decltype() required for GCC 7
  if (!value.Accept(writer))
    throw std::runtime_error{"dmitigr::rajson: to_stringified_json() accept error"};
  return std::string{buf.GetString(), buf.GetSize()};
}

/// @returns The instance of JSON document constructed by parsing the `input`.
inline rapidjson::Document to_document(const std::string_view input)
{
  rapidjson::Document result;
  result.Parse(input.data(), input.size());
  return result;
}

/// The centralized "namespace" for conversion algorithms implementations.
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

/// Full specialization for `bool`.
template<> struct Conversions<bool> final {
  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    if (value.IsBool())
      return value.GetBool();

    throw std::invalid_argument{"invalid source for bool"};
  }
};

/// Full specialization for `std::uint8_t`.
template<> struct Conversions<std::uint8_t> final {
  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    if (value.IsUint()) {
      const auto result = value.GetUint();
      if (result <= std::numeric_limits<std::uint8_t>::max())
        return static_cast<std::uint8_t>(result);
    }
    throw std::invalid_argument{"invalid source for std::uint8_t"};
  }
};

/// Full specialization for `std::uint16_t`.
template<> struct Conversions<std::uint16_t> final {
  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    if (value.IsUint()) {
      const auto result = value.GetUint();
      if (result <= std::numeric_limits<std::uint16_t>::max())
        return static_cast<std::uint16_t>(result);
    }
    throw std::invalid_argument{"invalid source for std::uint16_t"};
  }
};

/// Full specialization for `std::uint32_t`.
template<> struct Conversions<std::uint32_t> final {
  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    if (value.IsUint()) {
      const auto result = value.GetUint();
      if (result <= std::numeric_limits<std::uint32_t>::max())
        return static_cast<std::uint32_t>(result);
    }
    throw std::invalid_argument{"invalid source for std::uint32_t"};
  }
};

/// Full specialization for `std::uint64_t`.
template<> struct Conversions<std::uint64_t> final {
  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    if (value.IsUint64())
      return static_cast<std::uint64_t>(value.GetUint64());

    throw std::invalid_argument{"invalid source for std::uint64_t"};
  }
};

/// Full specialization for `std::int8_t`.
template<> struct Conversions<std::int8_t> final {
  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    if (value.IsInt()) {
      const auto result = value.GetInt();
      if (std::numeric_limits<std::int8_t>::min() <= result &&
        result <= std::numeric_limits<std::int8_t>::max())
        return static_cast<std::int8_t>(result);
    }
    throw std::invalid_argument{"invalid source for std::int8_t"};
  }
};

/// Full specialization for `std::int16_t`.
template<> struct Conversions<std::int16_t> final {
  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    if (value.IsInt()) {
      const auto result = value.GetInt();
      if (std::numeric_limits<std::int16_t>::min() <= result &&
        result <= std::numeric_limits<std::int16_t>::max())
        return static_cast<std::int16_t>(result);
    }
    throw std::invalid_argument{"invalid source for std::int16_t"};
  }
};

/// Full specialization for `std::int32_t`.
template<> struct Conversions<std::int32_t> final {
  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    if (value.IsInt()) {
      const auto result = value.GetInt();
      if (std::numeric_limits<std::int32_t>::min() <= result &&
        result <= std::numeric_limits<std::int32_t>::max())
        return static_cast<std::int32_t>(result);
    }
    throw std::invalid_argument{"invalid source for std::int32_t"};
  }
};

/// Full specialization for `std::int64_t`.
template<> struct Conversions<std::int64_t> final {
  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    if (value.IsInt64())
      return static_cast<std::int64_t>(value.GetInt64());

    throw std::invalid_argument{"invalid source for std::int64_t"};
  }
};

/// Full specialization for `float`.
template<> struct Conversions<float> final {
  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    if (value.IsFloat() || value.IsLosslessFloat())
      return value.GetFloat();

    throw std::invalid_argument{"invalid source for float"};
  }
};

/// Full specialization for `double`.
template<> struct Conversions<double> final {
  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    if (value.IsDouble() || value.IsLosslessDouble())
      return value.GetDouble();

    throw std::invalid_argument{"invalid source for double"};
  }
};

/// Full specialization for `std::string`.
template<>
struct Conversions<std::string> final {
  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    if (value.IsString())
      return std::string{value.GetString(), value.GetStringLength()};

    throw std::invalid_argument{"invalid source for std::string"};
  }
};

/// Full specialization for `std::string_view`.
template<>
struct Conversions<std::string_view> final {
  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    if (value.IsString())
      return std::string_view{value.GetString(), value.GetStringLength()};

    throw std::invalid_argument{"invalid source for std::string_view"};
  }
};

/// Partial specialization for `rapidjson::StringRef`.
template<class CharType>
struct Conversions<rapidjson::GenericStringRef<CharType>> final {
  static auto from(const std::string_view value)
  {
    return rapidjson::GenericStringRef<CharType>{value.data(), value.size()};
  }
};

/// Partial specialization for `rapidjson::GenericValue`.
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

#endif  // DMITIGR_RAJSON_CONVERSIONS_HPP
