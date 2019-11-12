# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_cefeika_libraries
  util dt
  fcgi http img mulf pgfe ttpl url)

set(dmitigr_cefeika_fcgi_deps util)
set(dmitigr_cefeika_http_deps util dt)
set(dmitigr_cefeika_img_deps  util)
set(dmitigr_cefeika_mulf_deps util)
set(dmitigr_cefeika_pgfe_deps util)
set(dmitigr_cefeika_ttpl_deps util)
set(dmitigr_cefeika_url_deps  util)
