# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

include(${CMAKE_CURRENT_LIST_DIR}/dmitigr.cmake)

# ------------------------------------------------------------------------------
# Library list
# ------------------------------------------------------------------------------

set(dmitigr_cefeika_libraries_all
  # Independent (or std only dependent)
  algo assert base filesystem mem
  # Third-party only dependent
  rajson
  #
  wscl
  #
  concur dt math fsmisc progpar reader rng sqlixx str testo ttpl
  #
  cfg jrpc mulf net os url uuid
  #
  fcgi http pgfe ws
  #
  web
  )

# ------------------------------------------------------------------------------
# Dependency lists
# ------------------------------------------------------------------------------

set(dmitigr_cefeika_algo_deps)
set(dmitigr_cefeika_assert_deps)
set(dmitigr_cefeika_cfg_deps assert filesystem reader str)
set(dmitigr_cefeika_concur_deps assert)
set(dmitigr_cefeika_dt_deps assert)
set(dmitigr_cefeika_base_deps)
set(dmitigr_cefeika_fcgi_deps assert filesystem math net)
set(dmitigr_cefeika_filesystem_deps)
set(dmitigr_cefeika_fsmisc_deps filesystem)
set(dmitigr_cefeika_http_deps assert dt net str)
set(dmitigr_cefeika_jrpc_deps assert math rajson str)
set(dmitigr_cefeika_math_deps assert)
set(dmitigr_cefeika_mem_deps)
set(dmitigr_cefeika_mulf_deps assert str)
set(dmitigr_cefeika_net_deps assert base filesystem os)
set(dmitigr_cefeika_os_deps assert filesystem progpar)
set(dmitigr_cefeika_pgfe_deps assert base filesystem mem net os str)
set(dmitigr_cefeika_progpar_deps assert filesystem)
set(dmitigr_cefeika_rajson_deps thirdparty_rapidjson)
set(dmitigr_cefeika_reader_deps assert filesystem)
set(dmitigr_cefeika_rng_deps assert)
set(dmitigr_cefeika_sqlixx_deps assert filesystem)
set(dmitigr_cefeika_str_deps assert)
set(dmitigr_cefeika_testo_deps assert)
set(dmitigr_cefeika_ttpl_deps assert)
set(dmitigr_cefeika_url_deps assert str)
set(dmitigr_cefeika_uuid_deps assert rng)
set(dmitigr_cefeika_web_deps assert fcgi filesystem http jrpc mulf reader str ttpl)
set(dmitigr_cefeika_ws_deps assert filesystem net thirdparty_usockets thirdparty_uwebsockets)
set(dmitigr_cefeika_wscl_deps assert thirdparty_uwsc)
set(dmitigr_cefeika_thirdparty_uwebsockets_deps thirdparty_usockets)

# ------------------------------------------------------------------------------
# Source type list
# ------------------------------------------------------------------------------

set(dmitigr_cefeika_source_types
  root_headers
  preprocessed_headers
  headers
  build_only_sources
  implementations
  cmake_sources
  cmake_unpreprocessed
  transunits)

# ------------------------------------------------------------------------------
# Dependency related stuff
# ------------------------------------------------------------------------------

function(dmitigr_cefeika_get_deps res_var lib)
  foreach(dep ${dmitigr_cefeika_${lib}_deps})
    # Getting dependencies of dep
    dmitigr_cefeika_get_deps(dep_deps ${dep})

    # Adding dependencies of dep to the result
    foreach(d ${dep_deps})
      if (NOT ${d} IN_LIST result)
        list(APPEND result ${d})
      endif()
    endforeach()

    # Adding dep itself to the result
    if (NOT ${dep} IN_LIST result)
      list(APPEND result ${dep})
    endif()
  endforeach()

  set(${res_var} ${result} PARENT_SCOPE)
endfunction()

# ------------------------------------------------------------------------------
# Target compile options
# ------------------------------------------------------------------------------

function(dmitigr_cefeika_target_compile_options t)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${t} PRIVATE
      -pedantic
      -Wall
      -Wextra
      -Winline
      -Winit-self
      -Wuninitialized
      -Wmaybe-uninitialized
      -Woverloaded-virtual
      -Wsuggest-override
      -Wlogical-op
      -Wswitch)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${t} PRIVATE
      -pedantic
      -Weverything
      -Wno-c++98-compat
      -Wno-c++98-compat-pedantic
      -Wno-documentation-unknown-command
      -Wno-disabled-macro-expansion
      -Wno-weak-vtables
      -Wno-ctad-maybe-unsupported
      -Wno-padded
      -Wno-exit-time-destructors
      -Wno-global-constructors
      -Wno-covered-switch-default
      -Wno-switch-enum # but -Wswitch still active!
      -Wno-unused-private-field
      -Wno-reserved-id-macro
      )
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${t} PRIVATE
      /W4
      /Zc:throwingNew,referenceBinding,noexceptTypes
      /errorReport:none
      /nologo
      /utf-8)
  endif()
endfunction()

# ------------------------------------------------------------------------------

macro(dmitigr_cefeika_set_library_info lib version_major version_minor description)
  if((${version_major} LESS 0) OR (${version_minor} LESS 0))
    message(FATAL_ERROR "Invalid major or minor version of ${lib} specified")
  endif()

  set(dmitigr_${lib}_name "${lib}")
  string(TOUPPER "${lib}" dmitigr_${lib}_NAME)

  string(SUBSTRING "${lib}" 0 1 n)
  string(TOUPPER "${n}" N)
  string(SUBSTRING "${lib}" 1 -1 ame)
  set(dmitigr_${lib}_Name "${N}${ame}")

  set(dmitigr_${lib}_version_major "${version_major}")
  set(dmitigr_${lib}_version_minor "${version_minor}")
  math(EXPR dmitigr_${lib}_version_major_hi "(${version_major} >> 16) & 0x0000ffff" OUTPUT_FORMAT DECIMAL)
  math(EXPR dmitigr_${lib}_version_major_lo "(${version_major}) & 0x0000ffff" OUTPUT_FORMAT DECIMAL)
  math(EXPR dmitigr_${lib}_version_minor_hi "(${version_minor} >> 16) & 0x0000ffff" OUTPUT_FORMAT DECIMAL)
  math(EXPR dmitigr_${lib}_version_minor_lo "(${version_minor}) & 0x0000ffff" OUTPUT_FORMAT DECIMAL)

  set(dmitigr_${lib}_description "${description}")
  set(dmitigr_${lib}_internal_name "dmitigr_${lib}")
  set(dmitigr_${lib}_product_name "Dmitigr ${dmitigr_${lib}_Name}")
endmacro()

# ------------------------------------------------------------------------------

macro(dmitigr_cefeika_propagate_library_settings lib)
  set(dmitigr_${lib}_name ${dmitigr_${lib}_name} PARENT_SCOPE)
  set(dmitigr_${lib}_NAME ${dmitigr_${lib}_NAME} PARENT_SCOPE)
  set(dmitigr_${lib}_Name ${dmitigr_${lib}_Name} PARENT_SCOPE)

  set(dmitigr_${lib}_version_major ${dmitigr_${lib}_version_major} PARENT_SCOPE)
  set(dmitigr_${lib}_version_minor ${dmitigr_${lib}_version_minor} PARENT_SCOPE)
  set(dmitigr_${lib}_version_major_hi ${dmitigr_${lib}_version_major_hi} PARENT_SCOPE)
  set(dmitigr_${lib}_version_major_lo ${dmitigr_${lib}_version_major_lo} PARENT_SCOPE)
  set(dmitigr_${lib}_version_minor_hi ${dmitigr_${lib}_version_minor_hi} PARENT_SCOPE)
  set(dmitigr_${lib}_version_minor_lo ${dmitigr_${lib}_version_minor_lo} PARENT_SCOPE)
  set(dmitigr_${lib}_description ${dmitigr_${lib}_description} PARENT_SCOPE)

  set(dmitigr_${lib}_internal_name ${dmitigr_${lib}_internal_name} PARENT_SCOPE)
  set(dmitigr_${lib}_product_name ${dmitigr_${lib}_product_name} PARENT_SCOPE)

  foreach(st ${dmitigr_cefeika_source_types})
    set(dmitigr_${lib}_${st} ${dmitigr_${lib}_${st}} PARENT_SCOPE)
  endforeach()

  foreach(suff public private interface)
    set(dmitigr_${lib}_target_link_libraries_${suff} ${dmitigr_${lib}_target_link_libraries_${suff}} PARENT_SCOPE)
    set(dmitigr_${lib}_target_compile_definitions_${suff} ${dmitigr_${lib}_target_compile_definitions_${suff}} PARENT_SCOPE)
    set(dmitigr_${lib}_target_include_directories_${suff} ${dmitigr_${lib}_target_include_directories_${suff}} PARENT_SCOPE)
  endforeach()
endmacro()

# ------------------------------------------------------------------------------

macro(dmitigr_cefeika_propagate_tests_settings lib)
  set(dmitigr_${lib}_tests ${dmitigr_${lib}_tests} PARENT_SCOPE)
  set(dmitigr_${lib}_tests_target_link_libraries ${dmitigr_${lib}_tests_target_link_libraries} PARENT_SCOPE)
  set(dmitigr_${lib}_tests_target_compile_definitions ${dmitigr_${lib}_tests_target_compile_definitions} PARENT_SCOPE)
endmacro()

# ------------------------------------------------------------------------------

macro(dmitigr_cefeika_set_library_info_lib_variables lib)
  set(dmitigr_lib_name ${dmitigr_${lib}_name})
  set(dmitigr_lib_NAME ${dmitigr_${lib}_NAME})
  set(dmitigr_lib_Name ${dmitigr_${lib}_Name})
  set(dmitigr_lib_version_major ${dmitigr_${lib}_version_major})
  set(dmitigr_lib_version_minor ${dmitigr_${lib}_version_minor})
  set(dmitigr_lib_version_major_hi ${dmitigr_${lib}_version_major_hi})
  set(dmitigr_lib_version_major_lo ${dmitigr_${lib}_version_major_lo})
  set(dmitigr_lib_version_minor_hi ${dmitigr_${lib}_version_minor_hi})
  set(dmitigr_lib_version_minor_lo ${dmitigr_${lib}_version_minor_lo})
  set(dmitigr_lib_description ${dmitigr_${lib}_description})
  set(dmitigr_lib_internal_name ${dmitigr_${lib}_internal_name})
  set(dmitigr_lib_product_name ${dmitigr_${lib}_product_name})
endmacro()
