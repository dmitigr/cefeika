# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(mulf 0 1 "Multipart/form-data library")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_mulf_headers
  form_data.hpp
  types_fwd.hpp
  )

set(dmitigr_mulf_implementations
  form_data.cpp
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_mulf_target_link_libraries_public dmitigr::str dmitigr::util)
set(dmitigr_mulf_target_link_libraries_interface dmitigr::str dmitigr::util)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(mulf)
