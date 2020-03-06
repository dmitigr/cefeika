# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(rng 0 1 "The random number generators library")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_rng_root_headers
  ../rng.hpp
  )

set(dmitigr_rng_headers
  rng.hpp
  str.hpp
  types_fwd.hpp
  )

set(dmitigr_rng_implementations
  str.cpp
  )

set(dmitigr_rng_transunits
  ../rng.cpp
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_rng_target_link_libraries_public dmitigr::util)
set(dmitigr_rng_target_link_libraries_interface dmitigr::util)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(rng)
