# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

enable_language(C)
set(C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

set(uwsc_headers
  buffer.h
  config.h
  log.h
  sha1.h
  utils.h
  uwsc.h
  )
set(uwsc_implementations
  buffer.c
  log.c
  sha1.c
  utils.c
  uwsc.c
  )

find_package(Ev REQUIRED)
set(uwsc_link_libraries_public ${Ev_LIBRARIES})
set(uwsc_include_directories_public ${Ev_INCLUDE_DIRS})

if(DMITIGR_CEFEIKA_OPENSSL)
  find_package(OpenSSL REQUIRED)
  set(uwsc_compile_definitions_public UWSC_SSL_SUPPORT UWSC_HAVE_OPENSSL)
  list(APPEND uwsc_link_libraries_public OpenSSL::SSL OpenSSL::Crypto)
  list(APPEND uwsc_headers ssl.h)
  list(APPEND uwsc_implementations ssl.c)
endif()

if(MSVC)
  list(APPEND uwsc_cflags /W0)
elseif(CMAKE_C_COMPILER_ID MATCHES "AppleClang|Clang|GNU")
  list(APPEND uwsc_cflags
    -fvisibility=hidden
    -Wall
    -Wextra
    #-Wstrict-prototypes
    -Wno-sign-compare
    -Wno-unused-parameter)
endif()

if(WIN32)
  list(APPEND uwsc_defines WIN32_LEAN_AND_MEAN)
else()
  list(APPEND uwsc_defines _FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE _GNU_SOURCE)
endif()

# ------------------------------------------------------------------------------

set(dmlib dmitigr_3rdparty_uwsc)

if(DMITIGR_CEFEIKA_HEADER_ONLY)
  # In header-only mode we build a static library to be exported.
  set(dmlib_type STATIC)
  set(dmlib_export_type "static")
else()
  # In not header-only mode build an object library which will be exported as
  # interface automatically. The object library will be included (directly
  # linked) into the our libraries. The interface definition of the object
  # library will contains compile definitions.
  set(dmlib_type OBJECT)
  set(dmlib_export_type "interface")
endif()

add_library(${dmlib} ${dmlib_type} ${uwsc_implementations} ${uwsc_headers})
set_target_properties(${dmlib}
  PROPERTIES
  OUTPUT_NAME "${dmlib}"
  LINKER_LANGUAGE "C"
  POSITION_INDEPENDENT_CODE True
  DEBUG_POSTFIX "d")
target_compile_definitions(${dmlib}
  PUBLIC ${uwsc_compile_definitions_public}
  PRIVATE ${uwsc_defines})
target_compile_options(${dmlib} PRIVATE ${uwsc_cflags})
target_include_directories(${dmlib} PUBLIC ${uwsc_include_directories_public})
target_link_libraries(${dmlib} PUBLIC ${uwsc_link_libraries_public})

if(DMITIGR_CEFEIKA_INSTALL_INCLUDES)
  install(FILES ${uwsc_headers}
    DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/3rdparty/uwsc")
endif()
if(DMITIGR_CEFEIKA_INSTALL_LIBS)
  install(TARGETS ${dmlib}
    EXPORT ${dmlib}_export
    ARCHIVE DESTINATION ${DMITIGR_CEFEIKA_LIB_INSTALL_DIR})
endif()
if(DMITIGR_CEFEIKA_INSTALL_CMAKE)
  install(EXPORT ${dmlib}_export
    DESTINATION "${DMITIGR_CEFEIKA_CMAKE_INSTALL_DIR}"
    FILE "${dmlib}_${dmlib_export_type}-config.cmake")
endif()
