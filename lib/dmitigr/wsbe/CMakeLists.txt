# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(wsbe 0 1 "Websockets backend")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_wsbe_root_headers
  ../wsbe.hpp
  )

set(dmitigr_wsbe_headers
  )

set(dmitigr_wsbe_implementations
  )

set(dmitigr_wsbe_transunits
  ../wsbe.cpp
  )

set(dmitigr_wsbe_tests
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_wsbe_target_link_libraries_public thirdparty_uwebsockets dmitigr::util)
set(dmitigr_wsbe_target_link_libraries_interface thirdparty_uwebsockets dmitigr::util)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(wsbe)
