# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(ws 0 1 "WebSocket library")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_ws_headers
  basics.hpp
  connection.hpp
  http_request.hpp
  listener.hpp
  listener_options.hpp
  types_fwd.hpp
  )

set(dmitigr_ws_implementations
  basics.cpp
  connection.cpp
  http_request.cpp
  listener.cpp
  listener_options.cpp
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

foreach (t public interface)
  set(dmitigr_ws_target_link_libraries_${t} dmitigr::net)
endforeach()
set(dmitigr_ws_target_link_libraries_private thirdparty_uwebsockets)

if(WIN32)
  set(dmitigr_ws_target_compile_definitions_private WIN32_LEAN_AND_MEAN)
endif()

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(ws)
