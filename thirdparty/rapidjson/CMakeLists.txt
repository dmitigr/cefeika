# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmlib dmitigr_thirdparty_rapidjson)

add_library(${dmlib} INTERFACE)

target_compile_definitions(${dmlib}
  INTERFACE
  RAPIDJSON_HAS_STDSTRING=1
  RAPIDJSON_NO_SIZETYPEDEFINE)

install(DIRECTORY "./"
  DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty/rapidjson"
  FILES_MATCHING PATTERN "*.h")
install(TARGETS ${dmlib}
  EXPORT ${dmlib}_export)
install(EXPORT ${dmlib}_export
  DESTINATION ${DMITIGR_CEFEIKA_CMAKE_INSTALL_DIR}
  FILE ${dmlib}_interface-config.cmake)
