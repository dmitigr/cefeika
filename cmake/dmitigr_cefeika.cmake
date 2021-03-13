# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_cefeika_libraries
  misc

  dt
  os
  rajson

  jrpc
  net

  fcgi
  http
  pgfe
  ws
  wscl

  web
  )

set(dmitigr_cefeika_misc_deps)

set(dmitigr_cefeika_dt_deps misc)
set(dmitigr_cefeika_os_deps misc)
set(dmitigr_cefeika_rajson_deps thirdparty_rapidjson)

set(dmitigr_cefeika_jrpc_deps rajson misc)
set(dmitigr_cefeika_net_deps os)

set(dmitigr_cefeika_fcgi_deps net)
set(dmitigr_cefeika_http_deps dt net)
set(dmitigr_cefeika_pgfe_deps net)
set(dmitigr_cefeika_ws_deps thirdparty_uwebsockets net)
set(dmitigr_cefeika_wscl_deps thirdparty_uwsc)

set(dmitigr_cefeika_web_deps fcgi http jrpc)

# ------------------------------------------------------------------------------

set(dmitigr_cefeika_thirdparty_rapidjson_deps)
set(dmitigr_cefeika_thirdparty_thirdparty_uv_deps)
set(dmitigr_cefeika_thirdparty_usockets_deps thirdparty_uv)
set(dmitigr_cefeika_thirdparty_uwebsockets_deps thirdparty_usockets)

set(dmitigr_cefeika_thirdparty_buffer_deps)
set(dmitigr_cefeika_thirdparty_uwsc_deps thirdparty_buffer)
