# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(thread 0 1 "Threading stuff")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_thread_root_headers
  ../thread.hpp
  )

set(dmitigr_thread_headers
  thread.hpp
  types_fwd.hpp
  )

set(dmitigr_thread_implementations
  thread.cpp
  )

set(dmitigr_thread_transunits
  ../thread.cpp
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_thread_target_link_libraries_public dmitigr::util)
set(dmitigr_thread_target_link_libraries_interface dmitigr::util)

#
# System
#

if(NOT DMITIGR_CEFEIKA_HEADER_ONLY)
  if (UNIX)
    list(APPEND dmitigr_thread_target_link_libraries_public pthread)
  endif()
else()
  if (UNIX)
    list(APPEND dmitigr_thread_target_link_libraries_interface pthread)
  endif()
endif()

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(thread)
