#!/bin/bash
# Copyright (C) Dmitry Igrishin

algo_deps=""
assert_deps=""
endianness_deps=""
enum_bitmask_deps=""
filesystem_deps=""
mem_deps=""
rajson_deps="thirdparty_rapidjson"
wscl_deps="assert thirdparty_uwsc"
concur_deps="assert"
dt_deps="assert"
math_deps="assert"
fsmisc_deps="filesystem"
progpar_deps="assert filesystem"
reader_deps="assert filesystem"
rng_deps="assert"
sqlixx_deps="assert filesystem"
str_deps="assert"
testo_deps="assert"
ttpl_deps="assert"
cfg_deps="assert filesystem reader str"
jrpc_deps="assert math thirdparty_rapidjson rajson str"
mulf_deps="assert str"
net_deps="assert endianness enum_bitmask filesystem progpar os"
os_deps="assert filesystem progpar"
url_deps="assert str"
uuid_deps="assert rng"
fcgi_deps="assert filesystem math endianness enum_bitmask progpar os net"
http_deps="assert dt endianness enum_bitmask filesystem progpar os net str"
pgfe_deps="assert enum_bitmask filesystem mem endianness progpar os net str"
ws_deps="assert filesystem endianness enum_bitmask progpar os net thirdparty_usockets thirdparty_uwebsockets"
web_deps="assert filesystem math endianness enum_bitmask progpar os net fcgi dt str http thirdparty_rapidjson rajson jrpc mulf reader ttpl"