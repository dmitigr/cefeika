# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmlib dmitigr_3rdparty_uwebsockets)

add_library(${dmlib} INTERFACE)
target_link_libraries(${dmlib} INTERFACE dmitigr_3rdparty_usockets)
target_compile_definitions(${dmlib} INTERFACE UWS_HTTPRESPONSE_NO_WRITEMARK)
if (DMITIGR_CEFEIKA_ZLIB)
  find_package(ZLIB REQUIRED)
  target_link_libraries(${dmlib} INTERFACE ZLIB::ZLIB)
else()
  target_compile_definitions(${dmlib} INTERFACE UWS_NO_ZLIB)
endif()

if(DMITIGR_CEFEIKA_INSTALL_INCLUDES)
  install(DIRECTORY "./"
    DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/3rdparty/uwebsockets"
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
endif()
if(DMITIGR_CEFEIKA_INSTALL_CMAKE)
  install(TARGETS ${dmlib}
    EXPORT ${dmlib}_export)
  install(EXPORT ${dmlib}_export
    DESTINATION ${DMITIGR_CEFEIKA_CMAKE_INSTALL_DIR}
    FILE ${dmlib}_interface-config.cmake)
endif()
