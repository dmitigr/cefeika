// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_BASICS_HPP
#define DMITIGR_HTTP_BASICS_HPP

#include <dmitigr/base/debug.hpp>

#include <string_view>

namespace dmitigr::http {

/**
 * @ingroup headers
 *
 * @brief A value of "SameSite" cookie attribute.
 */
enum class Same_site { strict, lax };

/**
 * @ingroup headers
 *
 * @returns The result of conversion of `str` to the value of type Same_site.
 *
 * @remarks The value of `str` is case-sensitive.
 */
inline Same_site to_same_site(const std::string_view str)
{
  if (str == "Strict")
    return Same_site::strict;
  else if (str == "Lax")
    return Same_site::lax;
  else
    DMITIGR_THROW_REQUIREMENT_VIOLATED(str == "Strict" || str == "Lax", std::invalid_argument);
}

/**
 * @ingroup headers
 *
 * @returns The result of conversion of `ss` to the instance of type `std::string`.
 */
inline std::string to_string(const Same_site ss)
{
  switch (ss) {
  case Same_site::strict: return "Strict";
  case Same_site::lax: return "Lax";
  }
  DMITIGR_ASSERT_ALWAYS(!true);
}

} // namespace dmitigr::http

#endif  // DMITIGR_HTTP_BASICS_HPP
