// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#include "dmitigr/http/basics.hpp"
#include "dmitigr/http/implementation_header.hpp"

#include <dmitigr/util/debug.hpp>

namespace dmitigr::http {

DMITIGR_HTTP_INLINE Same_site to_same_site(std::string_view str)
{
  if (str == "Strict")
    return Same_site::strict;
  else if (str == "Lax")
    return Same_site::lax;
  else
    DMITIGR_THROW_REQUIREMENT_VIOLATED(str == "Strict" || str == "Lax", std::invalid_argument);
}

DMITIGR_HTTP_INLINE std::string to_string(const Same_site ss)
{
  switch (ss) {
  case Same_site::strict: return "Strict";
  case Same_site::lax: return "Lax";
  }
  DMITIGR_ASSERT_ALWAYS(!true);
}

} // namespace dmitigr::http

#include "dmitigr/http/implementation_footer.hpp"
