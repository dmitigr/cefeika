# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

add_library(thirdparty_rapidjson INTERFACE)
target_include_directories(thirdparty_rapidjson INTERFACE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/thirdparty/rapidjson/include>
  $<INSTALL_INTERFACE:${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty>)

set(lib thirdparty_rapidjson)

install(DIRECTORY "include/"
  DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty")
install(TARGETS ${lib}
  EXPORT dmitigr_${lib}_export)
install(EXPORT dmitigr_${lib}_export
  DESTINATION ${DMITIGR_CEFEIKA_CMAKE_INSTALL_DIR}
  FILE dmitigr_${lib}_${export_file_suffix}-config.cmake)
