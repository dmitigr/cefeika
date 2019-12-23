# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(usockx 0 1 "The wrapper around uSockets")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_usockx_root_headers
  ../usockx.hpp
  )

set(dmitigr_usockx_headers
  )

set(dmitigr_usockx_implementations
  )

set(dmitigr_usockx_transunits
  ../usockx.cpp
  )

set(dmitigr_usockx_tests
  )

# ------------------------------------------------------------------------------
# Third-party dependencies
# ------------------------------------------------------------------------------

#
# uSockets
#

if(MSVC)
  list(APPEND usockets_cflags /W4)
elseif(CMAKE_C_COMPILER_ID MATCHES "AppleClang|Clang|GNU")
  list(APPEND usockets_cflags
    -fPIC
    -fvisibility=hidden
    --std=c99
    -Wall
    -Wextra
    #-Wstrict-prototypes
    -Wno-unused-parameter)
endif()

set(usockets_sources
  src/context.c
  src/loop.c
  src/socket.c
  src/eventing/libuv.c
  src/internal/internal.h
  src/internal/loop_data.h
  src/internal/eventing/libuv.h
  src/internal/networking/bsd.h
  )

if(WIN32)
  list(APPEND usockets_defines WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0600)
  list(APPEND usockets_libraries
    advapi32
    iphlpapi
    psapi
    shell32
    user32
    userenv
    ws2_32)
else()
  list(APPEND usockets_defines _FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE)
endif()

list(TRANSFORM usockets_sources PREPEND "${CMAKE_SOURCE_DIR}/thirdparty/uSockets/")

add_library(usockets_static OBJECT ${usockets_sources})
target_compile_definitions(usockets_static PRIVATE ${usockets_defines})
target_compile_options(usockets_static PRIVATE ${usockets_cflags})
target_include_directories(usockets_static
  PRIVATE "${CMAKE_SOURCE_DIR}/thirdparty/uSockets/include" "${CMAKE_SOURCE_DIR}/thirdparty/uSockets/src")
target_link_libraries(usockets_static ${usockets_libraries})

install(DIRECTORY "${CMAKE_SOURCE_DIR}/thirdparty/uSockets/include/"
  DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty")

set(usockets_include_dir "${CMAKE_INSTALL_PREFIX}/${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty")
list(APPEND dmitigr_usockx_target_include_directories_public "${usockets_include_dir}")
list(APPEND dmitigr_usockx_target_include_directories_interface "${usockets_include_dir}")

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

set(dmitigr_usockx_target_link_libraries_public dmitigr::util)
set(dmitigr_usockx_target_link_libraries_private usockets_static)

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(usockx)
