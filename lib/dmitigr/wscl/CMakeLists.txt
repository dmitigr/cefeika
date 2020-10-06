# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(wscl 0 1 "Asynchronous WebSocket client library")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_wscl_headers
  wscl.hpp
  )

set(dmitigr_wscl_implementations
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

if (NOT DMITIGR_CEFEIKA_HEADER_ONLY)
  set(suffix "public")
else()
  set(suffix "interface")
endif()

if(WIN32)
  set(dmitigr_wscl_target_compile_definitions_${suffix} WIN32_LEAN_AND_MEAN)
endif()

if(DMITIGR_CEFEIKA_WITH_OPENSSL)
  set(dmitigr_wscl_target_compile_definitions_private DMITIGR_CEFEIKA_WITH_OPENSSL)
endif()

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(wscl)
