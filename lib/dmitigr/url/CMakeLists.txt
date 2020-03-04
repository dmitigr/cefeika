# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(url 0 1 "The URL library")

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

set(dmitigr_url_tests qs1 qs2)
set(dmitigr_url_tests_target_link_libraries dmitigr::str)

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_url_target_link_libraries_public dmitigr::util)
set(dmitigr_url_target_link_libraries_interface dmitigr::util)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(url)
