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
  )

#
# System
#

if (UNIX)
  list(APPEND dmitigr_mp_target_link_libraries_interface pthread)
endif()

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(mp)
