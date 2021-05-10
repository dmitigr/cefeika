// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or sqlixx.hpp

#ifndef DMITIGR_SQLIXX_DATA_HPP
#define DMITIGR_SQLIXX_DATA_HPP

#include <sqlite3.h>

#include <cassert>
#include <utility>

namespace dmitigr::sqlixx {

/// A data.
template<typename T, unsigned char E>
struct Data final {
  static_assert((E == 0) || (E == SQLITE_UTF8) ||
    (E == SQLITE_UTF16LE) || (E == SQLITE_UTF16BE) || (E == SQLITE_UTF16),
    "invalid data encoding");

  /// An alias of deleter.
  using Deleter = sqlite3_destructor_type;

  /// The data type.
  using Type = T;

  /// The data size type.
  using Size = sqlite3_uint64;

  /// The data encoding.
  constexpr static unsigned char Encoding = E;

  /// The destructor.
  ~Data()
  {
    if (is_data_owner())
      deleter_(const_cast<T*>(data_));
  }

  /// The default constructor.
  Data() = default;

  /// The constructor.
  Data(const T* const data, const Size size,
    const Deleter deleter = SQLITE_STATIC) noexcept
    : data_{data}
    , size_{size}
    , deleter_{deleter}
  {}

  /// Non-copyable.
  Data(const Data&) = delete;

  /// Non-copyable.
  Data& operator=(const Data&) = delete;

  /// The move constructor.
  Data(Data&& rhs) noexcept
    : data_{rhs.data_}
    , size_{rhs.size_}
    , deleter_{rhs.deleter_}
  {
    rhs.data_ = {};
    rhs.size_ = {};
    rhs.deleter_ = SQLITE_STATIC;
  }

  /// The move assignment operator.
  Data& operator=(Data&& rhs) noexcept
  {
    if (this != &rhs) {
      Data tmp{std::move(rhs)};
      swap(tmp);
    }
    return *this;
  }

  /// The swap operation.
  void swap(Data& other) noexcept
  {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(deleter_, other.deleter_);
  }

  /// @returns The released data.
  T* release() noexcept
  {
    auto* const result = data_;
    deleter_ = SQLITE_STATIC;
    assert(!is_data_owner());
    Data{}.swap(*this);
    return result;
  }

  /// @returns The data.
  const T* data() const noexcept { return data_; }

  /// @returns The data size.
  Size size() const noexcept { return size_; }

  /// @returns The deleter.
  Deleter deleter() const noexcept { return deleter_; }

  /// @returns `true` if this instance is owns the data.
  bool is_data_owner() const noexcept
  {
    return deleter_ && (deleter_ != SQLITE_STATIC) && (deleter_ != SQLITE_TRANSIENT);
  }

  /// The data.
  const T* data_{};

  /// The data size.
  Size size_{};

  /// The deleter.
  Deleter deleter_{SQLITE_STATIC};
};

/// An alias of Blob type.
using Blob = Data<void, 0>;

/// An alias of UTF8 text type.
using Text_utf8 = Data<char, SQLITE_UTF8>;

/// An alias of UTF16 text type.
using Text_utf16 = Data<char, SQLITE_UTF16>;

/// An alias of UTF16LE text type.
using Text_utf16le = Data<char, SQLITE_UTF16LE>;

/// An alias of UTF16BE text type.
using Text_utf16be = Data<char, SQLITE_UTF16BE>;

} // namespace dmitigr::sqlixx

#endif  // DMITIGR_SQLIXX_DATA_HPP
