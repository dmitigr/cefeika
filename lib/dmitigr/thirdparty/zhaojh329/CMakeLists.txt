# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(buffer_sources
  buffer.c
  )
list(APPEND libs buffer)
if ("wscl" IN_LIST dmitigr_cefeika_libraries)
  set(uwsc_sources
    log.c
    sha1.c
    utils.c
    uwsc.c
    )
  list(APPEND libs uwsc)

  find_package(Ev REQUIRED)

  if(DMITIGR_CEFEIKA_OPENSSL)
    find_package(OpenSSL REQUIRED)
    list(APPEND uwsc_sources ssl.c)
  endif()
endif()

enable_language(C)
set(C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

if(MSVC)
  list(APPEND libs_cflags /W0)
elseif(CMAKE_C_COMPILER_ID MATCHES "AppleClang|Clang|GNU")
  list(APPEND libs_cflags
    -fvisibility=hidden
    -Wall
    -Wextra
    #-Wstrict-prototypes
    -Wno-sign-compare
    -Wno-unused-parameter)
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
  list(APPEND libs_defines WIN32_LEAN_AND_MEAN _WIN32_WINNT=${windows-version})
else()
  list(APPEND libs_defines _FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE _GNU_SOURCE)
endif()

# ------------------------------------------------------------------------------

set(uwsc_link_libraries_public ${Suggested_Ev_LIBRARIES} thirdparty_buffer)
set(uwsc_include_directories_build_interface ${Ev_INCLUDE_DIRS})
set(uwsc_include_directories_install_interface ${Ev_INCLUDE_DIRS})
if(DMITIGR_CEFEIKA_OPENSSL)
  set(uwsc_compile_definitions_public UWSC_SSL_SUPPORT UWSC_HAVE_OPENSSL)
  list(APPEND uwsc_link_libraries_public OpenSSL::SSL OpenSSL::Crypto)
endif()
foreach(lib ${libs})
  set(target thirdparty_${lib})
  add_library(${target} STATIC ${${lib}_sources})
  set_target_properties(${target}
    PROPERTIES
    OUTPUT_NAME "dmitigr_${target}"
    LINKER_LANGUAGE "C"
    POSITION_INDEPENDENT_CODE True
    DEBUG_POSTFIX "d")
  target_compile_definitions(${target}
    PUBLIC ${${lib}_compile_definitions_public}
    PRIVATE ${libs_defines})
  target_compile_options(${target} PRIVATE ${libs_cflags})
  target_include_directories(${target}
    PUBLIC
    $<BUILD_INTERFACE:${${lib}_include_directories_build_interface}>
    $<INSTALL_INTERFACE:${${lib}_include_directories_install_interface}>)
  target_link_libraries(${target} PUBLIC ${${lib}_link_libraries_public})

  install(DIRECTORY "include/"
    DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty")
  install(TARGETS ${target}
    EXPORT dmitigr_${target}_export
    ARCHIVE  DESTINATION ${DMITIGR_CEFEIKA_LIB_INSTALL_DIR})
  install(EXPORT dmitigr_${target}_export
    DESTINATION "${DMITIGR_CEFEIKA_CMAKE_INSTALL_DIR}"
    FILE "dmitigr_${target}_static-config.cmake")
endforeach()
