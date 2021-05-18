# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_mulf_tests valid)
set(dmitigr_mulf_tests_target_link_libraries)
dmitigr_cefeika_propagate_tests_settings(mulf)

add_custom_target(dmitigr_mulf_copy_test_resources ALL
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/mulf-form-data-valid1.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/mulf-form-data-invalid1.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/mulf-form-data-invalid2.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/mulf-form-data-invalid3.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/mulf-form-data-invalid4.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/mulf-form-data-invalid5.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  )
