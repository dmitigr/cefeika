# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(mulf 0 1 "The multipart/form-data library")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_mulf_root_headers
  ../mulf.hpp
  )

set(dmitigr_mulf_headers
  form_data.hpp
  types_fwd.hpp
  )

set(dmitigr_mulf_implementations
  form_data.cpp
  )

set(dmitigr_mulf_transunits
  ../mulf.cpp
  )

set(dmitigr_mulf_tests valid1)
set(dmitigr_mulf_tests_target_link_libraries dmitigr::fs)

# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------

add_custom_target(dmitigr_mulf_copy_test_resources ALL
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/test/mulf-form-data-valid1.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/test/mulf-form-data-invalid1.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/test/mulf-form-data-invalid2.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/test/mulf-form-data-invalid3.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/test/mulf-form-data-invalid4.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/test/mulf-form-data-invalid5.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_mulf_target_link_libraries_public dmitigr::util dmitigr::str)
set(dmitigr_mulf_target_link_libraries_interface dmitigr::util dmitigr::str)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(mulf)
