# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_util_tests net)
set(dmitigr_dt_tests timestamp)
set(dmitigr_fcgi_tests hello hellomt overload)
set(dmitigr_http_tests basics cookie date set_cookie)
set(dmitigr_mulf_tests valid1)
set(dmitigr_pgfe_tests benchmark_array_client benchmark_array_server
  benchmark_sql_string_replace composite connection connection_deferrable
  connection-err_in_mid connection_options connection_ssl conversions
  conversions_online data entity_vector hello_world problem ps sql_string sql_vector)
set(dmitigr_ttpl_tests llt)
set(dmitigr_url_tests qs1 qs2)

# Tests-specific linkage
if(UNIX)
  set(dmitigr_fcgi_tests_target_link_libraries pthread)
endif()

# ------------------------------------------------------------------------------

function(dmitigr_configure_test lib test)
  set(exe "${lib}-${test}")
  if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${exe}.cpp")
    set(is_unit_test TRUE)
    set(exe "${lib}-unit-${test}")
  else()
    set(is_unit_test FALSE)
  endif()
  set(src "${exe}.cpp")
  add_executable(${exe} ${src})
  target_link_libraries(${exe} dmitigr::util dmitigr::${lib} ${dmitigr_${lib}_tests_target_link_libraries})
  dmitigr_target_compile_options(${exe})
  if(is_unit_test)
    add_test(NAME ${exe} COMMAND ${exe})
  endif()
endfunction()

foreach(lib ${dmitigr_cefeika_libraries})
  foreach(test ${dmitigr_${lib}_tests})
    dmitigr_configure_test(${lib} ${test})
  endforeach()
endforeach()

# ------------------------------------------------------------------------------

if (WIN32)
  set(dmitigr_cefeika_resource_destination_dir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>")
elseif (UNIX)
  set(dmitigr_cefeika_resource_destination_dir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

add_custom_target(copy_test_resources ALL
  COMMAND cmake -E make_directory "${dmitigr_cefeika_resource_destination_dir}"

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

  COMMAND cmake -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/pgfe-unit-sql_vector.sql"
  "${dmitigr_cefeika_resource_destination_dir}"
  )
