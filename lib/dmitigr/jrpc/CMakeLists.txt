# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(jrpc 0 1 "The JSON RPC library")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_jrpc_root_headers
  ../jrpc.hpp
  )

set(dmitigr_jrpc_headers
  basics.hpp
  errc.hpp
  error.hpp
  request.hpp
  response.hpp
  response_dfn.hpp
  result.hpp
  std_system_error.hpp
  types_fwd.hpp
  )

set(dmitigr_jrpc_implementations
  basics.cpp
  errc.cpp
  error.cpp
  request.cpp
  response.cpp
  result.cpp
  std_system_error.cpp
  )

set(dmitigr_jrpc_transunits
  ../jrpc.cpp
  )

set(dmitigr_jrpc_tests request response)

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_jrpc_target_link_libraries_public dmitigr::rajson dmitigr::util)
set(dmitigr_jrpc_target_link_libraries_interface dmitigr::rajson dmitigr::util)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(jrpc)
