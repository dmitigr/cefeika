# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

enable_language(C)
set(C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

if (DMITIGR_CEFEIKA_WITH_OPENSSL)
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
if (DMITIGR_CEFEIKA_WITH_OPENSSL)
  list(APPEND usockets_sources src/crypto/openssl.c)
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
  PUBLIC LIBUS_USE_LIBUV)
target_compile_options(${lib} PRIVATE ${usockets_cflags})
target_include_directories(${lib}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty>
  PRIVATE
  src)
target_link_libraries(${lib} thirdparty_uv)
if (DMITIGR_CEFEIKA_WITH_OPENSSL)
  target_compile_definitions(${lib} PRIVATE LIBUS_USE_OPENSSL)
  target_link_libraries(${lib} OpenSSL::SSL OpenSSL::Crypto)
else()
  target_compile_definitions(${lib} PRIVATE LIBUS_NO_SSL)
endif()

install(DIRECTORY "include/"
  DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty")
install(DIRECTORY "src/internal"
  DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty")
install(TARGETS ${lib}
  EXPORT dmitigr_${lib}_export
  ARCHIVE  DESTINATION ${DMITIGR_CEFEIKA_LIB_INSTALL_DIR})
install(EXPORT dmitigr_${lib}_export
  DESTINATION "${DMITIGR_CEFEIKA_CMAKE_INSTALL_DIR}"
  FILE "dmitigr_${lib}_static-config.cmake")
