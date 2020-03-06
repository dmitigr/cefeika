# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(dt 0 1 "Date and time library")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_dt_root_headers
  ../dt.hpp
  )

set(dmitigr_dt_headers
  basics.hpp
  timestamp.hpp
  types_fwd.hpp
  )

set(dmitigr_dt_implementations
  basics.cpp
  timestamp.cpp
  )

set(dmitigr_dt_transunits
  ../dt.cpp
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_dt_target_link_libraries_public dmitigr::util)
set(dmitigr_dt_target_link_libraries_interface dmitigr::util)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(dt)
