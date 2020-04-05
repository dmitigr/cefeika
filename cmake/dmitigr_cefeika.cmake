# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_cefeika_libraries
  # Abstraction level 1
  base
  # Abstraction level 2
  algo
  fs
  math
  mem
  mp
  rajson
  rng
  str
  app
  dt
  img
  net
  os
  testo
  ttpl
  # Abstraction level 3
  cfg
  jrpc
  mulf
  url
  uuid
  fcgi
  http
  pgfe
  ws
  # Abstraction level 4
  web
  )

# Abstraction level 1
set(dmitigr_cefeika_base_deps)
# Abstraction level 2
set(dmitigr_cefeika_algo_deps base)
set(dmitigr_cefeika_fs_deps base)
set(dmitigr_cefeika_math_deps base)
set(dmitigr_cefeika_mem_deps base)
set(dmitigr_cefeika_mp_deps base)
set(dmitigr_cefeika_rajson_deps base)
set(dmitigr_cefeika_rng_deps base)
set(dmitigr_cefeika_str_deps base)
set(dmitigr_cefeika_app_deps base)
set(dmitigr_cefeika_dt_deps base)
set(dmitigr_cefeika_img_deps base)
set(dmitigr_cefeika_net_deps base)
set(dmitigr_cefeika_os_deps base)
set(dmitigr_cefeika_testo_deps base)
set(dmitigr_cefeika_ttpl_deps base)
# Abstraction level 3
set(dmitigr_cefeika_cfg_deps str base)
set(dmitigr_cefeika_jrpc_deps rajson str base)
set(dmitigr_cefeika_mulf_deps str base)
set(dmitigr_cefeika_url_deps str base)
set(dmitigr_cefeika_uuid_deps rng base)
set(dmitigr_cefeika_fcgi_deps net base)
set(dmitigr_cefeika_http_deps dt str base)
set(dmitigr_cefeika_pgfe_deps mem net str base)
set(dmitigr_cefeika_ws_deps net base)
# Abstraction level 4
set(dmitigr_cefeika_web_deps fcgi jrpc mulf pgfe ws ttpl url base)
