# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(uuid 0 1 "UUID processing")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_uuid_headers
  uuid.hpp
  types_fwd.hpp
  )

set(dmitigr_uuid_implementations
  uuid.cpp
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_uuid_target_link_libraries_public dmitigr::rng)
set(dmitigr_uuid_target_link_libraries_interface dmitigr::rng)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(uuid)
