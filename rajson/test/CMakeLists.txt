# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_rajson_tests conversions value_view)
set(dmitigr_rajson_tests_target_link_libraries dmitigr_filesystem dmitigr_str dmitigr_testo)

add_custom_target(dmitigr_rajson_copy_test_resources ALL
  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/rajson-unit-value_view.json"
  "${dmitigr_cefeika_resource_destination_dir}"
  )

dmitigr_cefeika_propagate_tests_settings(rajson)
