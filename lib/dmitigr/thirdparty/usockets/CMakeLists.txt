# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

enable_language(C)
set(C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

if (DMITIGR_CEFEIKA_OPENSSL)
  find_package(OpenSSL REQUIRED)
endif()

if(MSVC)
  list(APPEND usockets_cflags /W0)
elseif(CMAKE_C_COMPILER_ID MATCHES "AppleClang|Clang|GNU")
  list(APPEND usockets_cflags
    -fvisibility=hidden
    -Wall
    -Wextra
    #-Wstrict-prototypes
    -Wno-unused-parameter)
endif()

set(usockets_headers
  internal/internal.h
  internal/internal_dmitigr.h
  internal/loop_data.h
  internal/eventing/libuv.h
  internal/networking/bsd.h
  internal/networking/bsd_dmitigr.h
  )
set(usockets_sources
  bsd.c
  context.c
  loop.c
  socket.c
  socket_dmitigr.c
  eventing/libuv.c
  )
if (DMITIGR_CEFEIKA_OPENSSL)
  list(APPEND usockets_sources crypto/openssl.c crypto/sni_tree.cpp)
  list(APPEND usockets_cflags
    -fpermissive
    )
endif()

if(WIN32)
  if (CMAKE_SYSTEM_VERSION VERSION_GREATER 10) # Windows 10
    set(windows-version 0x0A00)
  elseif (CMAKE_SYSTEM_VERSION VERSION_GREATER 6.3) # Windows 8.1
    set(windows-version 0x0603)
  elseif (CMAKE_SYSTEM_VERSION VERSION_GREATER 6.2) # Windows 8
    set(windows-version 0x0602)
  elseif (CMAKE_SYSTEM_VERSION VERSION_GREATER 6.1) # Windows 7
    set(windows-version 0x0601)
  elseif (CMAKE_SYSTEM_VERSION VERSION_GREATER 6.0) # Windows Vista
    set(windows-version 0x0600)
  else()
    message(FATAL_ERROR "Windows Vista is the minimum version supported")
  endif()
  list(APPEND usockets_defines WIN32_LEAN_AND_MEAN _WIN32_WINNT=${windows-version})
else()
  list(APPEND usockets_defines _FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE)
endif()

# ------------------------------------------------------------------------------

set(target thirdparty_usockets)

add_library(${target} STATIC ${usockets_sources})
set_target_properties(${target}
  PROPERTIES
  OUTPUT_NAME "dmitigr_${target}"
  LINKER_LANGUAGE "C"
  POSITION_INDEPENDENT_CODE True
  DEBUG_POSTFIX "d")
target_compile_definitions(${target}
  PRIVATE ${usockets_defines}
  PUBLIC LIBUS_USE_LIBUV)
target_compile_options(${target} PRIVATE ${usockets_cflags})
target_include_directories(${target}
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(${target} PUBLIC thirdparty_uv)
if (DMITIGR_CEFEIKA_OPENSSL)
  target_compile_definitions(${target} PRIVATE LIBUS_USE_OPENSSL)
  target_link_libraries(${target} PUBLIC OpenSSL::SSL OpenSSL::Crypto)
else()
  target_compile_definitions(${target} PRIVATE LIBUS_NO_SSL)
endif()

install(FILES libusockets.h libusockets_dmitigr.h
  DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty/usockets")
install(TARGETS ${target}
  EXPORT dmitigr_${target}_export
  ARCHIVE  DESTINATION ${DMITIGR_CEFEIKA_LIB_INSTALL_DIR})
install(EXPORT dmitigr_${target}_export
  DESTINATION "${DMITIGR_CEFEIKA_CMAKE_INSTALL_DIR}"
  FILE "dmitigr_${target}_static-config.cmake")
