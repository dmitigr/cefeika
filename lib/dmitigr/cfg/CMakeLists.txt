# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(cfg 0 1 "The ... library")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_cfg_root_headers
  ../cfg.hpp
  )

set(dmitigr_cfg_headers
  flat.hpp
  types_fwd.hpp
  )

set(dmitigr_cfg_implementations
  flat.cpp
  )

set(dmitigr_cfg_transunits
  ../cfg.cpp
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_cfg_target_link_libraries_public dmitigr::util)
set(dmitigr_cfg_target_link_libraries_interface dmitigr::util)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(cfg)
