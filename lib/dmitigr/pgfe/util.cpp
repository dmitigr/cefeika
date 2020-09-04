// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#include "dmitigr/pgfe/util.hpp"

#include <locale>

namespace dmitigr::pgfe {

DMITIGR_PGFE_INLINE std::string unquote_identifier(const std::string_view identifier)
{
  enum { top, double_quote, adjacent_double_quote } state = top;

  std::string result;
  const std::string_view::size_type sz = identifier.size();
  for (std::string_view::size_type i = 0; i < sz; ++i) {
    const char c = identifier[i];
    if (state == top) {
      if (c != '"')
        result += std::tolower(c, std::locale{});
      else
        state = double_quote;
    } else if (state == double_quote) {
      if (c != '"')
        result += c;
      else
        state = (i + 1 < sz && identifier[i + 1] == '"') ? adjacent_double_quote : top;
    } else if (state == adjacent_double_quote) {
      result += c;
      state = double_quote;
    }
  }
  return result;
}

} // namespace dmitigr::pgfe
