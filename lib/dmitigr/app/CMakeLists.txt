# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(app 0 1 "The application support library")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_app_root_headers
  ../app.hpp
  )

set(dmitigr_app_headers
  program_parameters.hpp
  types_fwd.hpp
  )

set(dmitigr_app_implementations
  program_parameters.cpp
  )

set(dmitigr_app_transunits
  ../app.cpp
  )

set(dmitigr_app_tests program_parameters)

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_app_target_link_libraries_public dmitigr::util)
set(dmitigr_app_target_link_libraries_interface dmitigr::util)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(app)
