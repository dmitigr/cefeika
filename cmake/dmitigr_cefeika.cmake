# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_cefeika_libraries
  base
  testo

  algo
  app
  dt
  fs
  img
  math
  mem
  mp
  os
  rajson
  rng
  str
  ttpl

  net
  cfg
  jrpc
  mulf
  url
  uuid

  fcgi
  http
  pgfe
  ws

  web
  )

set(dmitigr_cefeika_base_deps)
set(dmitigr_cefeika_testo_deps)

set(dmitigr_cefeika_algo_deps base)
set(dmitigr_cefeika_app_deps os base)
set(dmitigr_cefeika_dt_deps base)
set(dmitigr_cefeika_fs_deps base)
set(dmitigr_cefeika_img_deps base)
set(dmitigr_cefeika_math_deps base)
set(dmitigr_cefeika_mem_deps base)
set(dmitigr_cefeika_mp_deps base)
set(dmitigr_cefeika_os_deps base)
set(dmitigr_cefeika_rajson_deps thirdparty_rapidjson base)
set(dmitigr_cefeika_rng_deps base)
set(dmitigr_cefeika_str_deps base)
set(dmitigr_cefeika_ttpl_deps base)

set(dmitigr_cefeika_cfg_deps str base)
set(dmitigr_cefeika_jrpc_deps rajson math str base)
set(dmitigr_cefeika_mulf_deps str base)
set(dmitigr_cefeika_net_deps os base)
set(dmitigr_cefeika_url_deps str base)
set(dmitigr_cefeika_uuid_deps rng base)

set(dmitigr_cefeika_fcgi_deps net math base)
set(dmitigr_cefeika_http_deps dt net str base)
set(dmitigr_cefeika_pgfe_deps mem net str base)
set(dmitigr_cefeika_ws_deps thirdparty_uwebsockets net base)

set(dmitigr_cefeika_web_deps fcgi http jrpc mulf ttpl url base)

# ------------------------------------------------------------------------------

set(dmitigr_cefeika_thirdparty_rapidjson_deps)
set(dmitigr_cefeika_thirdparty_thirdparty_uv_deps)
set(dmitigr_cefeika_thirdparty_usockets_deps thirdparty_uv)
set(dmitigr_cefeika_thirdparty_uwebsockets_deps thirdparty_usockets)
