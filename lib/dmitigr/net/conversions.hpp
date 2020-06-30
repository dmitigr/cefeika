// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or net.hpp

#ifndef DMITIGR_NET_CONVERSIONS_HPP
#define DMITIGR_NET_CONVERSIONS_HPP

#include <dmitigr/base/endianness.hpp>

#include <cstdint>
#include <stdexcept>
#include <type_traits>

namespace dmitigr::net {

/**
 * @brief Converts the raw `data` to the value of type `T` taking into
 * account the host's endianness.
 *
 * If the host architecture is big endian, then the `data` is copyied to
 * the result as is. Otherwise, the bytes of `data` is copied to the result
 * in reverse order.
 */
template<typename T>
T conv(const char* const data, const std::size_t data_size)
{
  DMITIGR_REQUIRE(data, std::invalid_argument);
  DMITIGR_REQUIRE(data_size <= sizeof(T), std::invalid_argument);
  T result{};
  const auto data_ubytes = reinterpret_cast<const unsigned char*>(data);
  const auto result_ubytes = reinterpret_cast<unsigned char*>(&result);
  using Counter = std::remove_const_t<decltype (data_size)>;
  switch (endianness()) {
  case Endianness::big:
    for (Counter i = 0; i < data_size; ++i)
      result_ubytes[sizeof(T) - data_size + i] = data_ubytes[i];
    break;
  case Endianness::little:
    for (Counter i = 0; i < data_size; ++i)
      result_ubytes[sizeof(T) - 1 - i] = data_ubytes[i];
    break;
  case Endianness::unknown:
    throw std::logic_error("unknown endianness");
  }
  return result;
}

/**
 * @returns The result of conversion of `value` to the value of type `Dest`
 * taking into account the host's endianness.
 */
template<typename Dest, typename Src>
inline Dest conv(const Src& value)
{
  static_assert(sizeof(Dest) >= sizeof(Src),
    "the size of destination type must not be less than the size of source type");
  const char* const data = reinterpret_cast<const char*>(&value);
  return conv<Dest>(data, sizeof(Src));
}

/// @overload
template<typename T>
inline T conv(const T& value)
{
  return conv<T, T>(value);
}

} // namespace dmitigr::net

#endif  // DMITIGR_NET_CONVERSIONS_HPP
