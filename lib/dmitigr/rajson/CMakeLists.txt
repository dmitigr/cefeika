# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(rajson 0 1 "The wrapper around RapidJSON library")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_rajson_root_headers
  ../rajson.hpp
  )

set(dmitigr_rajson_headers
  conversions.hpp
  flat.hpp
  fwd.hpp
  )

set(dmitigr_rajson_implementations
  conversions.cpp
  )

set(dmitigr_rajson_transunits
  ../rajson.cpp
  )

set(dmitigr_rajson_tests flat
  )

# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------

add_custom_target(dmitigr_rajson_copy_test_resources ALL
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/test/rajson-unit-flat.json"
  "${dmitigr_cefeika_resource_destination_dir}"
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_rajson_target_link_libraries_public thirdparty_rapidjson dmitigr::util)
set(dmitigr_rajson_target_link_libraries_interface thirdparty_rapidjson dmitigr::util)
foreach(t public interface)
  set(dmitigr_rajson_target_compile_definitions_${t}
    -DRAPIDJSON_HAS_STDSTRING
    -DRAPIDJSON_NO_SIZETYPEDEFINE)
endforeach()

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(rajson)
