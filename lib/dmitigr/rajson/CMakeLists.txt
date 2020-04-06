# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(rajson 0 1 "RapidJSON wrapper")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_rajson_headers
  conversions.hpp
  value_view.hpp
  fwd.hpp
  )

set(dmitigr_rajson_implementations
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

foreach(t interface)
  set(dmitigr_rajson_target_compile_definitions_${t}
    -DRAPIDJSON_HAS_STDSTRING
    -DRAPIDJSON_NO_SIZETYPEDEFINE)
endforeach()

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(rajson)
