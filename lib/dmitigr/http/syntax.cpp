// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#include "dmitigr/http/syntax.hpp"

namespace dmitigr::http {

DMITIGR_HTTP_INLINE bool is_valid_cookie_name(const std::string_view name)
{
  return !name.empty() &&
    std::all_of(cbegin(name), cend(name), [](const char ch) { return detail::rfc6265::is_valid_token_character(ch); });
}

DMITIGR_HTTP_INLINE bool is_valid_cookie_value(const std::string_view value)
{
  return std::all_of(cbegin(value), cend(value), [](const char ch) { return detail::rfc6265::is_valid_cookie_octet(ch); });
}

} // namespace dmitigr::http
