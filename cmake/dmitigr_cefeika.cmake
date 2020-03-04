# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_cefeika_libraries
  util
  algo
  math
  rng
  mem
  fs
  str
  thread
  uuid
  cfg
  net
  os
  dt rajson ws
  app fcgi http img jrpc mulf pgfe web ttpl url)

set(dmitigr_cefeika_algo_deps)
set(dmitigr_cefeika_app_deps fs util)
set(dmitigr_cefeika_cfg_deps math str util)
set(dmitigr_cefeika_dt_deps util)
set(dmitigr_cefeika_fcgi_deps net util)
set(dmitigr_cefeika_fs_deps util)
set(dmitigr_cefeika_http_deps dt util)
set(dmitigr_cefeika_img_deps  util)
set(dmitigr_cefeika_jrpc_deps rajson util)
set(dmitigr_cefeika_math_deps util)
set(dmitigr_cefeika_mem_deps)
set(dmitigr_cefeika_mulf_deps util)
set(dmitigr_cefeika_net_deps fs util)
set(dmitigr_cefeika_os_deps util)
set(dmitigr_cefeika_pgfe_deps net util)
set(dmitigr_cefeika_rng_deps util)
set(dmitigr_cefeika_rajson_deps util)
set(dmitigr_cefeika_str_deps rng util)
set(dmitigr_cefeika_ttpl_deps util)
set(dmitigr_cefeika_url_deps  util)
set(dmitigr_cefeika_uuid_deps math util)
set(dmitigr_cefeika_web_deps fcgi jrpc mulf pgfe ws ttpl url util)
set(dmitigr_cefeika_ws_deps net util)
