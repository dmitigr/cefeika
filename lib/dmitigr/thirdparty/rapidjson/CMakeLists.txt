# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(target thirdparty_rapidjson)

add_library(${target} INTERFACE)

target_compile_definitions(${target}
  INTERFACE
  RAPIDJSON_HAS_STDSTRING=1
  RAPIDJSON_NO_SIZETYPEDEFINE)

install(DIRECTORY "./"
  DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty/rapidjson"
  FILES_MATCHING PATTERN "*.h")
install(TARGETS ${target}
  EXPORT dmitigr_${target}_export)
install(EXPORT dmitigr_${target}_export
  DESTINATION ${DMITIGR_CEFEIKA_CMAKE_INSTALL_DIR}
  FILE dmitigr_${target}_interface-config.cmake)
