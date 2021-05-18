# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_cefeika_set_library_info(ws 0 1 "WebSocket library")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_ws_headers
  basics.hpp
  connection.hpp
  http_io.hpp
  http_request.hpp
  listener.hpp
  listener_options.hpp
  timer.hpp
  types_fwd.hpp
  util.hpp
  uwebsockets.hpp
  )

set(dmitigr_ws_implementations
  connection.cpp
  http_io.cpp
  http_request.cpp
  listener.cpp
  listener_options.cpp
  timer.cpp
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
  set(dmitigr_ws_target_compile_definitions_${suffix} WIN32_LEAN_AND_MEAN)
endif()

if(DMITIGR_CEFEIKA_OPENSSL)
  set(dmitigr_ws_target_compile_definitions_private DMITIGR_CEFEIKA_OPENSSL)
endif()

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_cefeika_propagate_library_settings(ws)