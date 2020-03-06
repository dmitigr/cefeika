# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(url 0 1 "URL processing")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_url_root_headers
  ../url.hpp
  )

set(dmitigr_url_headers
  query_string.hpp
  types_fwd.hpp
  )

set(dmitigr_url_implementations
  query_string.cpp
  )

set(dmitigr_url_transunits
  ../url.cpp
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_url_target_link_libraries_public dmitigr::str dmitigr::util)
set(dmitigr_url_target_link_libraries_interface dmitigr::str dmitigr::util)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(url)
