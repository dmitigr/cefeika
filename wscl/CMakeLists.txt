# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_cefeika_set_library_info(wscl 0 1 "Asynchronous WebSocket client library")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_wscl_headers
  wscl.hpp
  )

set(dmitigr_wscl_implementations
  wscl.cpp
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

if (NOT DMITIGR_CEFEIKA_HEADER_ONLY)
  set(suffix "public")
else()
  set(suffix "interface")
endif()

# dmitigr_3rdparty_uwsc must be exported anyway. If header-only mode is off,
# it will be exported as interface. Otherwise, it will be exported as static
# library. So, target_link_libraries cannot be private in this case.
set(dmitigr_wscl_target_link_libraries_${suffix} dmitigr_3rdparty_uwsc)

if(WIN32)
  set(dmitigr_wscl_target_compile_definitions_${suffix} WIN32_LEAN_AND_MEAN)
endif()

if(DMITIGR_CEFEIKA_OPENSSL)
  set(dmitigr_wscl_target_compile_definitions_private DMITIGR_CEFEIKA_OPENSSL)
endif()

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_cefeika_propagate_library_settings(wscl)
