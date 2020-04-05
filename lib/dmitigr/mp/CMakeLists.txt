# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(mp 0 1 "Multiprocessing stuff")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_mp_headers
  simple_thread_pool.hpp
  types_fwd.hpp
  )

set(dmitigr_mp_implementations
  simple_thread_pool.cpp
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_mp_target_link_libraries_public dmitigr::base)
set(dmitigr_mp_target_link_libraries_interface dmitigr::base)

#
# System
#

if(NOT DMITIGR_CEFEIKA_HEADER_ONLY)
  if (UNIX)
    list(APPEND dmitigr_mp_target_link_libraries_public pthread)
  endif()
else()
  if (UNIX)
    list(APPEND dmitigr_mp_target_link_libraries_interface pthread)
  endif()
endif()

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(mp)
