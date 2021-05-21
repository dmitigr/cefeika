# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

enable_language(C)
set(C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

if (DMITIGR_CEFEIKA_OPENSSL)
  find_package(OpenSSL REQUIRED)
endif()

find_package(Uv REQUIRED)

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
  internal/loop_data.h
  internal/eventing/libuv.h
  internal/networking/bsd.h
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
  list(APPEND usockets_sources crypto/openssl.c)
  add_library(dmitigr_3rdparty_usockets_sni_tree OBJECT crypto/sni_tree.cpp)
  list(APPEND usockets_sni_tree dmitigr_3rdparty_usockets_sni_tree)
endif()

if(WIN32)
  list(APPEND usockets_defines WIN32_LEAN_AND_MEAN)
else()
  list(APPEND usockets_defines _FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE)
endif()

# ------------------------------------------------------------------------------

set(dmlib dmitigr_3rdparty_usockets)

add_library(${dmlib} STATIC ${usockets_sources})
set_target_properties(${dmlib}
  PROPERTIES
  OUTPUT_NAME "${dmlib}"
  LINKER_LANGUAGE "C"
  POSITION_INDEPENDENT_CODE True
  DEBUG_POSTFIX "d")
target_compile_definitions(${dmlib}
  PRIVATE ${usockets_defines}
  PUBLIC LIBUS_USE_LIBUV)
target_compile_options(${dmlib} PRIVATE ${usockets_cflags})
target_include_directories(${dmlib}
  PUBLIC ${Uv_INCLUDE_DIRS}
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(${dmlib}
  PUBLIC ${Uv_LIBRARIES} ${Uv_EXTRA_LIBRARIES}
  PRIVATE ${usockets_sni_tree})
if (DMITIGR_CEFEIKA_OPENSSL)
  target_compile_definitions(${dmlib} PRIVATE LIBUS_USE_OPENSSL)
  target_link_libraries(${dmlib} PUBLIC OpenSSL::SSL OpenSSL::Crypto)
else()
  target_compile_definitions(${dmlib} PRIVATE LIBUS_NO_SSL)
endif()

if(DMITIGR_CEFEIKA_INSTALL_INCLUDES)
  install(FILES libusockets.h libusockets_dmitigr.h
    DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/3rdparty/usockets")
endif()
if(DMITIGR_CEFEIKA_INSTALL_LIBS)
  install(TARGETS ${dmlib} ${usockets_sni_tree}
    EXPORT ${dmlib}_export
    ARCHIVE  DESTINATION ${DMITIGR_CEFEIKA_LIB_INSTALL_DIR})
endif()
if(DMITIGR_CEFEIKA_INSTALL_CMAKE)
  install(EXPORT ${dmlib}_export
    DESTINATION "${DMITIGR_CEFEIKA_CMAKE_INSTALL_DIR}"
    FILE "${dmlib}_static-config.cmake")
endif()
