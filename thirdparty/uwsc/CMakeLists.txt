# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(lib thirdparty_uwsc)

enable_language(C)
set(C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

find_package(Ev REQUIRED)

if (DMITIGR_CEFEIKA_OPENSSL)
  find_package(OpenSSL REQUIRED)
endif()

if(MSVC)
  list(APPEND ${lib}_cflags /W0)
elseif(CMAKE_C_COMPILER_ID MATCHES "AppleClang|Clang|GNU")
  list(APPEND ${lib}_cflags
    -fvisibility=hidden
    -Wall
    -Wextra
    #-Wstrict-prototypes
    -Wno-unused-parameter)
endif()

set(${lib}_sources
  log.c
  sha1.c
  utils.c
  uwsc.c
  )
if (DMITIGR_CEFEIKA_OPENSSL)
  list(APPEND ${lib}_sources ssl.c)
endif()
list(TRANSFORM ${lib}_sources PREPEND src/)

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
  list(APPEND ${lib}_defines WIN32_LEAN_AND_MEAN _WIN32_WINNT=${windows-version})
else()
  list(APPEND ${lib}_defines _FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE _GNU_SOURCE)
endif()

# ------------------------------------------------------------------------------

add_library(${lib} STATIC ${${lib}_sources})
set_target_properties(${lib}
  PROPERTIES
  OUTPUT_NAME "dmitigr_${lib}"
  LINKER_LANGUAGE "C"
  POSITION_INDEPENDENT_CODE True
  DEBUG_POSTFIX "d")
target_compile_definitions(${lib}
  PRIVATE ${${lib}_defines})
target_compile_options(${lib} PRIVATE ${${lib}_cflags})
target_include_directories(${lib}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${Ev_INCLUDE_DIRS}>
  $<INSTALL_INTERFACE:${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty>
  $<INSTALL_INTERFACE:${Ev_INCLUDE_DIRS}>
  PRIVATE
  src)

target_link_libraries(${lib} PUBLIC ${Suggested_Ev_LIBRARIES} thirdparty_buffer)
if (DMITIGR_CEFEIKA_OPENSSL)
  target_compile_definitions(${lib} PUBLIC UWSC_SSL_SUPPORT PRIVATE UWSC_HAVE_OPENSSL)
  target_link_libraries(${lib} PUBLIC OpenSSL::SSL OpenSSL::Crypto)
endif()

install(DIRECTORY "include/"
  DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty")
install(TARGETS ${lib}
  EXPORT dmitigr_${lib}_export
  ARCHIVE  DESTINATION ${DMITIGR_CEFEIKA_LIB_INSTALL_DIR})
install(EXPORT dmitigr_${lib}_export
  DESTINATION "${DMITIGR_CEFEIKA_CMAKE_INSTALL_DIR}"
  FILE "dmitigr_${lib}_static-config.cmake")
