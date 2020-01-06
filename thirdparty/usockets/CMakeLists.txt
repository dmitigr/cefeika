# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

if(MSVC)
  list(APPEND usockets_cflags /W0)
elseif(CMAKE_C_COMPILER_ID MATCHES "AppleClang|Clang|GNU")
  list(APPEND usockets_cflags
    -fvisibility=hidden
    --std=c11
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
else()
  list(APPEND usockets_defines _FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE)
endif()

# ------------------------------------------------------------------------------

set(lib thirdparty_usockets)

add_library(${lib} STATIC ${usockets_sources})
set_target_properties(${lib}
  PROPERTIES
  OUTPUT_NAME "dmitigr_${lib}"
  LINKER_LANGUAGE "C"
  POSITION_INDEPENDENT_CODE True
  DEBUG_POSTFIX "d")
target_compile_definitions(${lib}
  PRIVATE ${usockets_defines}
  PUBLIC LIBUS_USE_LIBUV LIBUS_NO_SSL)
target_compile_options(${lib} PRIVATE ${usockets_cflags})
target_include_directories(${lib}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/thirdparty/usockets/include>
  $<INSTALL_INTERFACE:${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty>
  PRIVATE
  src)
target_link_libraries(${lib} thirdparty_uv)

install(DIRECTORY "include/"
  DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty")
install(TARGETS ${lib}
  EXPORT dmitigr_${lib}_export
  ARCHIVE  DESTINATION ${DMITIGR_CEFEIKA_LIB_INSTALL_DIR})
install(EXPORT dmitigr_${lib}_export
  DESTINATION "${DMITIGR_CEFEIKA_CMAKE_INSTALL_DIR}"
  FILE "dmitigr_${lib}_${export_file_suffix}-config.cmake")
