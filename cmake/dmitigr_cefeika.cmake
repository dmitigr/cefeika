# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_cefeika_libraries "dt;os;rajson;jrpc;net;fcgi;http;pgfe;sqlixx;ws;wscl;web")

set(dmitigr_cefeika_cfg_deps filesystem reader str)
set(dmitigr_cefeika_fsmisc_deps filesystem)
set(dmitigr_cefeika_progpar_deps filesystem)
set(dmitigr_cefeika_reader_deps filesystem)
set(dmitigr_cefeika_uuid_deps rng)

set(dmitigr_cefeika_rajson_deps thirdparty_rapidjson)
set(dmitigr_cefeika_fcgi_deps net)
set(dmitigr_cefeika_http_deps dt net)
set(dmitigr_cefeika_jrpc_deps rajson)
set(dmitigr_cefeika_net_deps os)
set(dmitigr_cefeika_pgfe_deps net)
set(dmitigr_cefeika_web_deps fcgi http jrpc)
set(dmitigr_cefeika_ws_deps thirdparty_uwebsockets net)
set(dmitigr_cefeika_wscl_deps thirdparty_uwsc)
set(dmitigr_cefeika_thirdparty_uwebsockets_deps thirdparty_usockets)
