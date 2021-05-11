# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_librarian_lib Uv)
set(${dmitigr_librarian_lib}_include_names uv.h)
if(BUILD_SHARED_LIBS)
  set(${dmitigr_librarian_lib}_release_library_names uv)
else()
  set(${dmitigr_librarian_lib}_release_library_names uv_a)
endif()
set(${dmitigr_librarian_lib}_library_paths ${LIBUV_LIB_PREFIX})
set(${dmitigr_librarian_lib}_include_paths ${LIBUV_INCLUDE_PREFIX})

include(dmitigr_librarian)

if(NOT Uv_FOUND)
  return()
endif()

if(DEFINED uv_libraries)
  set(dmitigr_uv_libraries_stashed ${uv_libraries})
  unset(uv_libraries)
endif()
if(WIN32)
  list(APPEND uv_libraries
    psapi
    user32
    advapi32
    iphlpapi
    userenv
    ws2_32)
else()
  if(NOT CMAKE_SYSTEM_NAME MATCHES "Android|OS390|QNX")
    # TODO: This should be replaced with find_package(Threads) if possible
    # Android has pthread as part of its c library, not as a separate
    # libpthread.so.
    list(APPEND uv_libraries pthread)
  endif()
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "AIX")
  list(APPEND uv_libraries perfstat)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
  list(APPEND uv_libraries dl)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  list(APPEND uv_libraries dl rt)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
  list(APPEND uv_libraries kvm)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "OS390")
  list(APPEND uv_libraries -Wl,xplink)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
  list(APPEND uv_libraries kstat nsl sendfile socket)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Haiku")
  list(APPEND uv_libraries bsd network)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "QNX")
  list(APPEND uv_libraries socket)
endif()
set(${dmitigr_librarian_lib}_EXTRA_LIBRARIES ${uv_libraries})
if(DEFINED dmitigr_uv_libraries_stashed)
  set(uv_libraries ${dmitigr_uv_libraries_stashed})
  unset(dmitigr_uv_libraries_stashed)
endif()
