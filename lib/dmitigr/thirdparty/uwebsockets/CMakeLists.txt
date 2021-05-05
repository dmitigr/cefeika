# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(lib thirdparty_uwebsockets)

set(uwebsockets_headers
  f2/function2.hpp
  App.h
  AsyncSocketData.h
  AsyncSocket.h
  BloomFilter.h
  HttpContextData.h
  HttpContext.h
  HttpParser.h
  HttpResponseData.h
  HttpResponse.h
  HttpRouter.h
  LoopData.h
  Loop.h
  PerMessageDeflate.h
  ProxyParser.h
  TopicTree.h
  Utilities.h
  WebSocketContextData.h
  WebSocketContext.h
  WebSocketData.h
  WebSocketExtensions.h
  WebSocket.h
  WebSocketHandshake.h
  WebSocketProtocol.h
  )

add_library(${lib} INTERFACE)
target_link_libraries(${lib} INTERFACE thirdparty_usockets)
target_compile_definitions(${lib} INTERFACE UWS_HTTPRESPONSE_NO_WRITEMARK)
if (DMITIGR_CEFEIKA_ZLIB)
  find_package(ZLIB REQUIRED)
  target_link_libraries(${lib} INTERFACE ZLIB::ZLIB)
else()
  target_compile_definitions(${lib} INTERFACE UWS_NO_ZLIB)
endif()

install(FILES ${uwebsockets_headers}
  DESTINATION "${DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR}/dmitigr/thirdparty/uwebsockets")
install(TARGETS ${lib}
  EXPORT dmitigr_${lib}_export)
install(EXPORT dmitigr_${lib}_export
  DESTINATION ${DMITIGR_CEFEIKA_CMAKE_INSTALL_DIR}
  FILE dmitigr_${lib}_interface-config.cmake)
