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
  fwd.hpp
  )

set(dmitigr_rajson_implementations
  conversions.cpp
  )

set(dmitigr_rajson_transunits
  ../rajson.cpp
  )

set(dmitigr_rajson_tests
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_rajson_target_link_libraries_public dmitigr::util)
set(dmitigr_rajson_target_link_libraries_interface dmitigr::util)
foreach(t public interface)
  set(dmitigr_rajson_target_compile_definitions_${t}
    -DRAPIDJSON_HAS_STDSTRING
    -DRAPIDJSON_NO_SIZETYPEDEFINE)
endforeach()

install(DIRECTORY "${CMAKE_SOURCE_DIR}/thirdparty/rapidjson/include"
  DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty/rapidjson")

set(rapidjson_include_dir "${CMAKE_INSTALL_PREFIX}/${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty/rapidjson/include")
list(APPEND dmitigr_rajson_target_include_directories_public "${rapidjson_include_dir}")
list(APPEND dmitigr_rajson_target_include_directories_interface "${rapidjson_include_dir}")

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(rajson)
