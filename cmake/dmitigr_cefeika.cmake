# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_cefeika_libraries
  # Abstraction level 1
  util
  # Abstraction level 2
  algo
  fs
  math
  mem
  mp
  rajson
  rng
  str
  # Abstraction level 3
  app
  cfg
  dt
  img
  jrpc
  mulf
  net
  os
  ttpl
  url
  uuid
  # Abstraction level 4
  fcgi
  http
  pgfe
  ws
  # Abstraction level 5
  web
  )

# Abstraction level 1
set(dmitigr_cefeika_util_deps)
# Abstraction level 2
set(dmitigr_cefeika_algo_deps util)
set(dmitigr_cefeika_fs_deps util)
set(dmitigr_cefeika_math_deps util)
set(dmitigr_cefeika_mem_deps util)
set(dmitigr_cefeika_mp_deps util)
set(dmitigr_cefeika_rajson_deps util)
set(dmitigr_cefeika_rng_deps util)
set(dmitigr_cefeika_str_deps util)
# Abstraction level 3
set(dmitigr_cefeika_app_deps
  fs util)
set(dmitigr_cefeika_cfg_deps
  fs str util)
set(dmitigr_cefeika_dt_deps
  util)
set(dmitigr_cefeika_img_deps
  util)
set(dmitigr_cefeika_jrpc_deps
  rajson str util)
set(dmitigr_cefeika_mulf_deps
  str util)
set(dmitigr_cefeika_net_deps
  fs util)
set(dmitigr_cefeika_os_deps
  fs util)
set(dmitigr_cefeika_ttpl_deps
  util)
set(dmitigr_cefeika_url_deps
  str util)
set(dmitigr_cefeika_uuid_deps
  rng util)
# Abstraction level 4
set(dmitigr_cefeika_fcgi_deps
  net fs util)
set(dmitigr_cefeika_http_deps
  dt str util)
set(dmitigr_cefeika_pgfe_deps
  mem net str util)
set(dmitigr_cefeika_ws_deps
  net util)
# Abstraction level 5
set(dmitigr_cefeika_web_deps
  fcgi jrpc mulf pgfe ws ttpl url util)
