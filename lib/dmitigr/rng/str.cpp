// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rng.hpp

#include "dmitigr/rng/rng.hpp"
#include "dmitigr/rng/str.hpp"
#include "dmitigr/rng/implementation_header.hpp"

#include "dmitigr/util/debug.hpp"

namespace dmitigr::rng {

DMITIGR_RNG_INLINE std::string random_string(const std::string& palette, const std::string::size_type size)
{
  std::string result;
  result.resize(size);
  if (const auto pallete_size = palette.size()) {
    using Counter = std::remove_const_t<decltype (pallete_size)>;
    for (Counter i = 0; i < size; ++i)
      result[i] = palette[rng::cpp_pl_3rd(pallete_size)];
  }
  return result;
}

DMITIGR_RNG_INLINE std::string random_string(const char beg, const char end, const std::string::size_type size)
{
  DMITIGR_ASSERT(beg < end);
  std::string result;
  result.resize(size);
  const auto length = end - beg;
  using Counter = std::remove_const_t<decltype (size)>;
  for (Counter i = 0; i < size; ++i) {
    result[i] = static_cast<char>((rng::cpp_pl_3rd(end) % length) + beg);
  }
  return result;
}

} // namespace dmitigr::rng

#include "dmitigr/rng/implementation_footer.hpp"
