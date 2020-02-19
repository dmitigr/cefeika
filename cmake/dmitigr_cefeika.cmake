# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_cefeika_libraries
  util dt rajson ws
  app fcgi http img jrpc mulf pgfe process ttpl url)

set(dmitigr_cefeika_app_deps util)
set(dmitigr_cefeika_dt_deps util)
set(dmitigr_cefeika_fcgi_deps util)
set(dmitigr_cefeika_http_deps dt util)
set(dmitigr_cefeika_img_deps  util)
set(dmitigr_cefeika_jrpc_deps rajson util)
set(dmitigr_cefeika_mulf_deps util)
set(dmitigr_cefeika_pgfe_deps util)
set(dmitigr_cefeika_process_deps util)
set(dmitigr_cefeika_rajson_deps util)
set(dmitigr_cefeika_ttpl_deps util)
set(dmitigr_cefeika_url_deps  util)
set(dmitigr_cefeika_ws_deps util)
