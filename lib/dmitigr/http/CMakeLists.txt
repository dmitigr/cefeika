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

set(dmitigr_http_root_headers
  ../http.hpp
  )

set(dmitigr_http_headers
  basics.hpp
  cookie.hpp
  date.hpp
  header.hpp
  set_cookie.hpp
  syntax.hpp
  types_fwd.hpp
  )

set(dmitigr_http_implementations
  basics.cpp
  cookie.cpp
  date.cpp
  set_cookie.cpp
  syntax.cpp
  )

set(dmitigr_http_transunits
  ../http.cpp
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_http_target_link_libraries_public dmitigr::net dmitigr::dt dmitigr::util)
set(dmitigr_http_target_link_libraries_interface dmitigr::net dmitigr::dt dmitigr::util)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(http)
