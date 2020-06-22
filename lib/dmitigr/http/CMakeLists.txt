# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(http 0 1 "HTTP library")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_http_headers
  basics.hpp
  client.hpp
  connection.hpp
  cookie.hpp
  date.hpp
  exceptions.hpp
  errc.hpp
  header.hpp
  server.hpp
  set_cookie.hpp
  std_system_error.hpp
  syntax.hpp
  types_fwd.hpp
  )

set(dmitigr_http_implementations
  )

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(http)
