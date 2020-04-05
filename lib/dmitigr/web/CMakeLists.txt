# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(web 0 1 "Web utilities")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_web_headers
  v1.hpp
  types_fwd.hpp
  )

set(dmitigr_web_implementations
  v1.cpp
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

foreach (t public interface)
  set(dmitigr_web_target_link_libraries_${t}
    dmitigr::fcgi
    dmitigr::http
    dmitigr::jrpc
    dmitigr::mulf
    dmitigr::ttpl)
endforeach()

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(web)
