# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

enable_language(C)
set(C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

set(buffer_headers
  buffer.h
  )
set(buffer_implementations
  buffer.c
  )
list(APPEND libs buffer)
if ("wscl" IN_LIST dmitigr_cefeika_libraries)
  set(uwsc_headers
    config.h
    log.h
    sha1.h
    utils.h
    uwsc.h
    )
  set(uwsc_implementations
    log.c
    sha1.c
    utils.c
    uwsc.c
    )
  list(APPEND libs uwsc)

  find_package(Ev REQUIRED)

  if(DMITIGR_CEFEIKA_OPENSSL)
    find_package(OpenSSL REQUIRED)
    list(APPEND uwsc_headers ssl.h)
    list(APPEND uwsc_implementations ssl.c)
  endif()
endif()

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
  list(APPEND libs_defines WIN32_LEAN_AND_MEAN)
else()
  list(APPEND libs_defines _FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE _GNU_SOURCE)
endif()

# ------------------------------------------------------------------------------

set(uwsc_link_libraries_public ${Suggested_Ev_LIBRARIES} dmitigr_thirdparty_buffer)
set(uwsc_include_directories_public ${Ev_INCLUDE_DIRS})
if(DMITIGR_CEFEIKA_OPENSSL)
  set(uwsc_compile_definitions_public UWSC_SSL_SUPPORT UWSC_HAVE_OPENSSL)
  list(APPEND uwsc_link_libraries_public OpenSSL::SSL OpenSSL::Crypto)
endif()
foreach(lib ${libs})
  set(dmlib dmitigr_thirdparty_${lib})
  add_library(${dmlib} STATIC ${${lib}_implementations} ${${lib}_headers})
  set_target_properties(${dmlib}
    PROPERTIES
    OUTPUT_NAME "${dmlib}"
    LINKER_LANGUAGE "C"
    POSITION_INDEPENDENT_CODE True
    DEBUG_POSTFIX "d")
  target_compile_definitions(${dmlib}
    PUBLIC ${${lib}_compile_definitions_public}
    PRIVATE ${libs_defines})
  target_compile_options(${dmlib} PRIVATE ${libs_cflags})
  target_include_directories(${dmlib}
    PUBLIC ${${lib}_include_directories_public})
  target_link_libraries(${dmlib} PUBLIC ${${lib}_link_libraries_public})

  install(FILES ${${lib}_headers}
    DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty/zhaojh329")
  install(TARGETS ${dmlib}
    EXPORT ${dmlib}_export
    ARCHIVE  DESTINATION ${DMITIGR_CEFEIKA_LIB_INSTALL_DIR})
  install(EXPORT ${dmlib}_export
    DESTINATION "${DMITIGR_CEFEIKA_CMAKE_INSTALL_DIR}"
    FILE "${dmlib}_static-config.cmake")
endforeach()
