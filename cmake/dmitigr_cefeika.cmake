# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_cefeika_libraries
  math
  testo
  util

  algo
  dt
  img
  mem
  mp
  os
  rajson
  rng
  str
  ttpl

  net
  jrpc
  mulf
  url
  uuid

  fcgi
  http
  pgfe
  ws
  wscl

  web
  )

set(dmitigr_cefeika_math_deps)
set(dmitigr_cefeika_testo_deps)
set(dmitigr_cefeika_util_deps)

set(dmitigr_cefeika_algo_deps util)
set(dmitigr_cefeika_dt_deps util)
set(dmitigr_cefeika_img_deps util)
set(dmitigr_cefeika_mem_deps util)
set(dmitigr_cefeika_mp_deps util)
set(dmitigr_cefeika_os_deps util)
set(dmitigr_cefeika_rajson_deps thirdparty_rapidjson)
set(dmitigr_cefeika_rng_deps util)
set(dmitigr_cefeika_str_deps util)
set(dmitigr_cefeika_ttpl_deps util)

set(dmitigr_cefeika_jrpc_deps rajson math str)
set(dmitigr_cefeika_mulf_deps str util)
set(dmitigr_cefeika_net_deps os util)
set(dmitigr_cefeika_url_deps str util)
set(dmitigr_cefeika_uuid_deps rng util)

set(dmitigr_cefeika_fcgi_deps net math util)
set(dmitigr_cefeika_http_deps dt net str util)
set(dmitigr_cefeika_pgfe_deps mem net str util)
set(dmitigr_cefeika_ws_deps thirdparty_uwebsockets net util)
set(dmitigr_cefeika_wscl_deps thirdparty_uwsc)

set(dmitigr_cefeika_web_deps fcgi http jrpc mulf ttpl url util)

# ------------------------------------------------------------------------------

set(dmitigr_cefeika_thirdparty_rapidjson_deps)
set(dmitigr_cefeika_thirdparty_thirdparty_uv_deps)
set(dmitigr_cefeika_thirdparty_usockets_deps thirdparty_uv)
set(dmitigr_cefeika_thirdparty_uwebsockets_deps thirdparty_usockets)

set(dmitigr_cefeika_thirdparty_buffer_deps)
set(dmitigr_cefeika_thirdparty_uwsc_deps thirdparty_buffer)
