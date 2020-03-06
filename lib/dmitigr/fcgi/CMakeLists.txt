# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(fcgi 1 0 "FastCGI implementation")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_fcgi_root_headers
  ../fcgi.hpp
  )

set(dmitigr_fcgi_headers
  basics.hpp
  connection.hpp
  listener.hpp
  listener_options.hpp
  server_connection.hpp
  streambuf.hpp
  streams.hpp
  types_fwd.hpp
  )

set(dmitigr_fcgi_implementations
  basics.cpp
  listener.cpp
  listener_options.cpp
  server_connection.cpp
  streambuf.cpp
  streams.cpp
  )

set(dmitigr_fcgi_transunits
  ../fcgi.cpp
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_fcgi_target_link_libraries_public dmitigr::math dmitigr::net dmitigr::util)
set(dmitigr_fcgi_target_link_libraries_interface dmitigr::math dmitigr::net dmitigr::util)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(fcgi)
