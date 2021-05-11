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
set(uwsc_link_libraries_public ${Suggested_Ev_LIBRARIES})
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

set(dmlib dmitigr_thirdparty_uwsc)
add_library(${dmlib} STATIC ${uwsc_implementations} ${uwsc_headers})
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

install(FILES ${uwsc_headers}
  DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty/zhaojh329")
install(TARGETS ${dmlib}
  EXPORT ${dmlib}_export
  ARCHIVE  DESTINATION ${DMITIGR_CEFEIKA_LIB_INSTALL_DIR})
install(EXPORT ${dmlib}_export
  DESTINATION "${DMITIGR_CEFEIKA_CMAKE_INSTALL_DIR}"
  FILE "${dmlib}_static-config.cmake")
