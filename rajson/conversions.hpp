// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rajson.hpp

#ifndef DMITIGR_RAJSON_CONVERSIONS_HPP
#define DMITIGR_RAJSON_CONVERSIONS_HPP

#include "fwd.hpp"
#include "error.hpp"
#include "../3rdparty/rapidjson/document.h"
#include "../3rdparty/rapidjson/schema.h"
#include "../3rdparty/rapidjson/stringbuffer.h"
#include "../3rdparty/rapidjson/writer.h"

#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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
  const rapidjson::ParseResult pr{result.Parse(input.data(), input.size())};
  if (!pr)
    throw Parse_exception{pr};
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
  using Result = rapidjson::GenericValue<Encoding, Allocator>;

  static auto from(Result&& value, Allocator& alloc) noexcept
  {
    (void)alloc;
    return std::move(value);
  }

  template<typename T>
  static std::enable_if_t<
    std::is_arithmetic_v<std::decay_t<T>>,
    Result
    >
  from(const T value, Allocator& alloc)
  {
    (void)alloc;
    return Result{value};
  }

  static auto from(const char* const value, Allocator& alloc)
  {
    // Don't copy `value` to result.
    (void)alloc;
    return Result{value, alloc};
  }

  static auto from(const std::string_view value, Allocator& alloc)
  {
    // Don't copy `value` to result.
    (void)alloc;
    return Result{value.data(), value.size()};
  }

  static auto from(const std::string& value, Allocator& alloc)
  {
    // Copy `value` to result.
    return Result{value, alloc};
  }

  template<typename T>
  static auto from(const std::vector<std::optional<T>>& value, Allocator& alloc)
  {
    Result result{rapidjson::kArrayType};
    result.Reserve(value.size(), alloc);
    for (const auto& val : value)
      result.PushBack(val ? to<Result>(*val, alloc) : Result{}, alloc);
    return result;
  }

  template<typename T>
  static auto from(const std::vector<T>& value, Allocator& alloc)
  {
    Result result{rapidjson::kArrayType};
    result.Reserve(value.size(), alloc);
    for (const auto& val : value)
      result.PushBack(val, alloc);
    return result;
  }
};

/// Partial specialization for `std::vector<std::optional<T>>`.
template<typename T>
struct Conversions<std::vector<std::optional<T>>> final {
  using Result = std::vector<std::optional<T>>;

  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    if (value.IsArray()) {
      const auto arr = value.GetArray();
      Result result;
      result.reserve(arr.Size());
      for (const auto& val : arr)
        result.emplace_back(!val.IsNull() ? to<T>(val) : std::optional<T>{});
      return result;
    }

    throw std::invalid_argument{"invalid source for std::vector<std::optional<T>>"};
  }
};

/// Partial specialization for `std::vector<T>`.
template<typename T>
struct Conversions<std::vector<T>> final {
  using Result = std::vector<T>;

  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    if (value.IsArray()) {
      const auto arr = value.GetArray();
      Result result;
      result.reserve(arr.Size());
      for (const auto& val : arr) {
        if (val.IsNull())
          throw std::invalid_argument{"NULL cannot be an element of std::vector<T>"};
        result.emplace_back(to<T>(val));
      }
      return result;
    }

    throw std::invalid_argument{"invalid source for std::vector<T>"};
  }
};

} // namespace dmitigr::rajson

#endif  // DMITIGR_RAJSON_CONVERSIONS_HPP
