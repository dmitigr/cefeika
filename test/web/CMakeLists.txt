# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_web_tests mulf_valid proof url)
set(dmitigr_web_tests_target_link_libraries)
dmitigr_propagate_tests_settings(web)

add_custom_target(dmitigr_web_copy_test_resources ALL
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/web-mulf-form-data-valid1.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/web-mulf-form-data-invalid1.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/web-mulf-form-data-invalid2.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/web-mulf-form-data-invalid3.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/web-mulf-form-data-invalid4.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/web-mulf-form-data-invalid5.txt"
  "${dmitigr_cefeika_resource_destination_dir}"
  )
