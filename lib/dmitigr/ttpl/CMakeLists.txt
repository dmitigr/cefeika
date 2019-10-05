# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(ttpl 0 1 "The text templates library")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_ttpl_root_headers
  ../ttpl.hpp
  )

set(dmitigr_ttpl_headers
  logic_less_template.hpp
  types_fwd.hpp
  )

set(dmitigr_ttpl_implementations
  logic_less_template.cpp
  )

set(dmitigr_ttpl_transunits
  ../ttpl.cpp
  )

set(dmitigr_ttpl_tests llt)

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_ttpl_target_link_libraries_public dmitigr::util)
set(dmitigr_ttpl_target_link_libraries_interface dmitigr::util)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(ttpl)
