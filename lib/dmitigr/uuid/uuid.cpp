// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or uuid.hpp

#include "dmitigr/uuid/uuid.hpp"
#include "dmitigr/rng/rng.hpp"
#include "dmitigr/util/debug.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>

namespace dmitigr::uuid::detail {

class iUuid final : public Uuid {
public:
  iUuid()
  {
    // Filling the data with random bytes.
    {
      constexpr auto minimum = static_cast<unsigned char>(1);
      constexpr auto maximum = std::numeric_limits<unsigned char>::max();
      for (std::size_t i = 0; i < sizeof (data_.raw_); ++i)
        data_.raw_[i] = rng::cpp_pl_3rd(minimum, maximum);
    }

    /*
     * Setting magic numbers for a "version 4" (pseudorandom) UUID.
     * See http://tools.ietf.org/html/rfc4122#section-4.4
     */
    data_.rep_.time_hi_and_version_ = (data_.rep_.time_hi_and_version_ & 0x0fff) | 0x4000;
    data_.rep_.clock_seq_hi_and_reserved_ = (data_.rep_.clock_seq_hi_and_reserved_ & 0x3f) | 0x80;

    DMITIGR_ASSERT(is_invariant_ok());
  }

  std::string to_string() const override
  {
    constexpr std::size_t buf_size = 36;
    char buf[buf_size + 1];
    const int count = std::snprintf(buf, sizeof (buf),
      "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
      data_.rep_.time_low_,
      data_.rep_.time_mid_,
      data_.rep_.time_hi_and_version_,
      data_.rep_.clock_seq_hi_and_reserved_,
      data_.rep_.clock_seq_low_,
      data_.rep_.node_[0],
      data_.rep_.node_[1],
      data_.rep_.node_[2],
      data_.rep_.node_[3],
      data_.rep_.node_[4],
      data_.rep_.node_[5]);
    DMITIGR_ASSERT(count == buf_size);
    return std::string{buf, buf_size};
  }

private:
  union {
    struct {
      std::uint32_t time_low_;
      std::uint16_t time_mid_;
      std::uint16_t time_hi_and_version_;
      std::uint8_t clock_seq_hi_and_reserved_;
      std::uint8_t clock_seq_low_;
      std::uint8_t node_[6];
    } rep_;
    unsigned char raw_[16]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  } data_;

  bool is_invariant_ok() const noexcept
  {
    return std::any_of(std::cbegin(data_.raw_), std::cend(data_.raw_), [](const auto b) { return b != 0; });
  }
};

} // namespace dmitigr::uuid::detail

namespace dmitigr::uuid {

DMITIGR_UUID_INLINE std::unique_ptr<Uuid> Uuid::make()
{
  return std::make_unique<detail::iUuid>();
}

} // namespace dmitigr::uuid
