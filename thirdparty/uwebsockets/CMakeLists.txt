# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(lib thirdparty_uwebsockets)

add_library(${lib} INTERFACE)
target_include_directories(${lib} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/thirdparty/uwebsockets/include>
  $<INSTALL_INTERFACE:${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty>)
target_link_libraries(${lib} INTERFACE thirdparty_usockets)

install(DIRECTORY "include/"
  DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty")
install(TARGETS ${lib}
  EXPORT dmitigr_${lib}_export)
install(EXPORT dmitigr_${lib}_export
  DESTINATION ${DMITIGR_CEFEIKA_CMAKE_INSTALL_DIR}
  FILE dmitigr_${lib}_${export_file_suffix}-config.cmake)
