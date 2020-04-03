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
  cookie.hpp
  date.hpp
  exceptions.hpp
  errc.hpp
  header.hpp
  set_cookie.hpp
  std_system_error.hpp
  syntax.hpp
  types_fwd.hpp
  )

set(dmitigr_http_implementations
  basics.cpp
  cookie.cpp
  date.cpp
  exceptions.cpp
  errc.cpp
  set_cookie.cpp
  std_system_error.cpp
  syntax.cpp
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_http_target_link_libraries_public dmitigr::net dmitigr::dt dmitigr::str dmitigr::util)
set(dmitigr_http_target_link_libraries_interface dmitigr::net dmitigr::dt dmitigr::str dmitigr::util)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(http)
