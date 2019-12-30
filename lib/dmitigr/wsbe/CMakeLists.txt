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
  basics.hpp
  connection.hpp
  listener.hpp
  listener_options.hpp
  server_connection.hpp
  types_fwd.hpp
  )

set(dmitigr_wsbe_implementations
  basics.cpp
  listener.cpp
  listener_options.cpp
  server_connection.cpp
  )

set(dmitigr_wsbe_transunits
  ../wsbe.cpp
  )

set(dmitigr_wsbe_tests
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

foreach (t public interface)
  set(dmitigr_wsbe_target_link_libraries_${t} thirdparty_uwebsockets dmitigr::util)
endforeach()

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(wsbe)
