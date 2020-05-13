# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(jrpc 0 1 "JSON RPC implementation")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_jrpc_headers
  basics.hpp
  errc.hpp
  request.hpp
  response.hpp
  std_system_error.hpp
  types_fwd.hpp
  )

set(dmitigr_jrpc_implementations
  )

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(jrpc)
