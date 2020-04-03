# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(net 0 1 "Networking library")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_net_headers
  descriptor.hpp
  exceptions.hpp
  net.hpp
  types_fwd.hpp
  )

set(dmitigr_net_implementations
  exceptions.cpp
  net.cpp
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_net_target_link_libraries_public dmitigr::fs dmitigr::os dmitigr::util)
set(dmitigr_net_target_link_libraries_interface dmitigr::fs dmitigr::os dmitigr::util)

if(NOT DMITIGR_CEFEIKA_HEADER_ONLY)
  if (WIN32)
    list(APPEND dmitigr_net_target_link_libraries_public Ws2_32.lib)
  endif()
else()
  if (WIN32)
    list(APPEND dmitigr_net_target_link_libraries_interface Ws2_32.lib)
  endif()
endif()

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(net)
