# -*- cmake -*-
# Copyright (C) 2021 Dmitry Igrishin
#
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.

# ------------------------------------------------------------------------------
# Library list
# ------------------------------------------------------------------------------

set(dmitigr_cefeika_libraries_all
  # Independent (or std only dependent)
  algo assert base crc filesystem mem
  # Third-party only dependent
  rajson
  #
  wscl
  #
  concur dt math fsmisc progpar reader rng sqlixx str testo ttpl
  #
  cfg jrpc mulf net os url uuid
  #
  fcgi http pgfe ws
  #
  web
  )

# ------------------------------------------------------------------------------
# Dependency lists
# ------------------------------------------------------------------------------

set(dmitigr_cefeika_algo_deps)
set(dmitigr_cefeika_assert_deps)
set(dmitigr_cefeika_base_deps)
set(dmitigr_cefeika_cfg_deps assert filesystem reader str)
set(dmitigr_cefeika_concur_deps assert)
set(dmitigr_cefeika_dt_deps assert)
set(dmitigr_cefeika_fcgi_deps assert filesystem math net)
set(dmitigr_cefeika_filesystem_deps)
set(dmitigr_cefeika_fsmisc_deps filesystem)
set(dmitigr_cefeika_http_deps assert dt net str)
set(dmitigr_cefeika_jrpc_deps assert math rajson str)
set(dmitigr_cefeika_math_deps assert)
set(dmitigr_cefeika_mem_deps)
set(dmitigr_cefeika_mulf_deps assert str)
set(dmitigr_cefeika_net_deps assert base filesystem os)
set(dmitigr_cefeika_os_deps assert filesystem progpar)
set(dmitigr_cefeika_pgfe_deps assert base filesystem mem net os str)
set(dmitigr_cefeika_progpar_deps assert filesystem)
set(dmitigr_cefeika_rajson_deps 3rdparty_rapidjson)
set(dmitigr_cefeika_reader_deps assert filesystem)
set(dmitigr_cefeika_rng_deps assert)
set(dmitigr_cefeika_sqlixx_deps assert filesystem)
set(dmitigr_cefeika_str_deps assert)
set(dmitigr_cefeika_testo_deps assert)
set(dmitigr_cefeika_ttpl_deps assert)
set(dmitigr_cefeika_url_deps assert str)
set(dmitigr_cefeika_uuid_deps assert rng)
set(dmitigr_cefeika_web_deps assert fcgi filesystem http jrpc mulf reader str ttpl)
set(dmitigr_cefeika_ws_deps assert filesystem net 3rdparty_uwebsockets)
set(dmitigr_cefeika_wscl_deps assert 3rdparty_uwsc)
set(dmitigr_cefeika_3rdparty_uwebsockets_deps 3rdparty_usockets)
