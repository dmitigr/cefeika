# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(algo 0 1 "Algorithms library")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_algo_root_headers
  ../algo.hpp
  )

set(dmitigr_algo_headers
  algo.hpp
  types_fwd.hpp
  )

set(dmitigr_algo_implementations
  )

set(dmitigr_algo_transunits
  ../algo.cpp
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_algo_target_link_libraries_public dmitigr::util)
set(dmitigr_algo_target_link_libraries_interface dmitigr::util)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(algo)
