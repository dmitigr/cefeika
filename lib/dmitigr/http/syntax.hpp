// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_SYNTAX_HPP
#define DMITIGR_HTTP_SYNTAX_HPP

#include "dmitigr/http/dll.hpp"

#include <algorithm>
#include <locale>
#include <string>

namespace dmitigr::http {

/**
 * @ingroup headers
 *
 * @returns `true` if the specified `name` is a valid cookie name, or
 * `false` otherwise.
 */
DMITIGR_HTTP_API bool is_valid_cookie_name(std::string_view name);

/**
 * @ingroup headers
 *
 * @returns `true` if the specified `value` is a valid cookie value, or
 * `false` otherwise.
 */
DMITIGR_HTTP_API bool is_valid_cookie_value(std::string_view value);

namespace detail {

inline bool is_ctl(const char c)
{
  return (0 <= c && c <= 31) || (c == 127);
}

inline bool is_whitespace(const char c)
{
  return c == ' ' || c == '\t';
}

namespace rfc6265 {

/**
 * @internal
 *
 * @returns `true` if `c` is a valid token character according to
 * https://tools.ietf.org/html/rfc6265#section-4.1.1, or `false` otherwise.
 */
inline bool is_valid_token_character(const char c)
{
  constexpr char separators[] = {'(', ')', '<', '>', '@', ',', ';', ':', '\\', '"', '/', '[', ']', '?', '=', '{', '}', ' ', '\t'};
  static const std::locale l{"C"};
  return std::isalnum(c, l) || c == '_' || c == '-' ||
    (!is_ctl(c) && std::none_of(std::cbegin(separators), std::cend(separators), [c](const char ch) { return c == ch; }));
}

/**
 * @internal
 *
 * @returns `true` if `c` is a valid cookie octet according to
 * https://tools.ietf.org/html/rfc6265#section-4.1.1, or `false` otherwise.
 */
inline bool is_valid_cookie_octet(const char c)
{
  constexpr char forbidden[] = {'"', ',', ';', '\\'};
  static const std::locale l{"C"};
  return std::isalnum(c, l) || (!is_ctl(c) && !is_whitespace(c) &&
    std::none_of(std::cbegin(forbidden), std::cend(forbidden), [c](const char ch) { return c == ch;  }));
}

} // namespace rfc6265

} // namespace detail

} // namespace dmitigr::http

#ifdef DMITIGR_HTTP_HEADER_ONLY
#include "dmitigr/http/syntax.cpp"
#endif

#endif  // DMITIGR_HTTP_SYNTAX_HPP
