# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(process 0 1 "The library to work with OS processes")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_process_root_headers
  ../process.hpp
  )

set(dmitigr_process_headers
  detach.hpp
  info.hpp
  types_fwd.hpp
  )

set(dmitigr_process_implementations
  detach.cpp
  info.cpp
  )

set(dmitigr_process_transunits
  ../process.cpp
  )

set(dmitigr_process_tests)
if (NOT WIN32)
  list(APPEND dmitigr_process_tests detach)
endif()

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_process_target_link_libraries_public dmitigr::util)
set(dmitigr_process_target_link_libraries_interface dmitigr::util)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(process)
