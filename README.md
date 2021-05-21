# C++ toolkit for backend development

Dmitigr Cefeika (hereinafter referred to as Cefeika) includes:

  - [dt] - library to work with date and time
  - [fcgi] - powerful FastCGI implementation (server)
  - [http] - HTTP library
  - [jrpc] - [JSON-RPC 2.0][json-rpc2] implementation
  - [net] - networking library
  - [os] - OS interaction
  - [pgfe] - powerful client API for [PostgreSQL]
  - [rajson] - [RapidJSON] wrapper
  - [sqlixx] - powerful client API for [SQLite]
  - [web] - Web frameworks
  - [ws] - WebSocket server library
  - [wscl] - WebSocket client library
  - miscellaneous stuff (from basics to URL processing to concurrency)

All of these libraries can be used as shared libraries, static libraries or
header-only libraries.

**Most of these libraries are in *work in progress* state! Participations and
contributions of any kind are welcome!**

Some of these libraries are available as standalone versions:

  - [Dmitigr Fcgi][dmitigr_fcgi];
  - [Dmitigr Pgfe][dmitigr_pgfe].

Any feedback are [welcome][dmitigr_mail]. Donations are also [welcome][dmitigr_paypal].

## Third-party dependencies which are not shipped with Cefeika:

- [CMake] build system version 3.13+;
- C++17 compiler ([GCC] 7.4+ or [Microsoft Visual C++][Visual_Studio] 15.7+);
- [libpq] library for [pgfe];
- [libuv] library for [ws];
- [OpenSSL] library (optionally) for [ws];
- [zlib] library (optionally) for [ws];
- [libev] library for [wscl];

## Third-party software which are shipped with Cefeika

|Name|Source|
|:---|:------|
|RapidJSON|https://github.com/dmitigr/rapidjson/tree/master|
|uSockets|https://github.com/dmitigr/uSockets/tree/master|
|uWebSockets|https://github.com/dmitigr/uWebSockets/tree/master|
|libuwsc|https://github.com/dmitigr/libuwsc/tree/master|

## CMake options

The table below (one may need to use horizontal scrolling for full view)
contains variables which can be passed to [CMake] for customization.

|CMake variable|Possible values|Default on Unix|Default on Windows|
|:-------------|:--------------|:--------------|:-----------------|
|**The flag to use libc++ with Clang**||||
|DMITIGR_CLANG_USE_LIBCPP|On \| Off|On|On|
|**The flag to only install the header-only libraries**||||
|DMITIGR_CEFEIKA_HEADER_ONLY|On \| Off|Off|Off|
|**The flag to build the tests**||||
|DMITIGR_CEFEIKA_TESTS|On \| Off|On|On|
|**The flag to link to OpenSSL**||||
|DMITIGR_CEFEIKA_OPENSSL|On \| Off|Off|Off|
|**The flag to link to zlib**||||
|DMITIGR_CEFEIKA_ZLIB|On \| Off|Off|Off|
|**The flag to build the shared libraries**||||
|BUILD_SHARED_LIBS|On \| Off|Off|Off|
|**The flag to be verbose upon build**||||
|CMAKE_VERBOSE_MAKEFILE|On \| Off|On|On|
|**The type of the build**||||
|CMAKE_BUILD_TYPE|Debug \| Release \| RelWithDebInfo \| MinSizeRel|Debug|Debug|
|**Installation directories**||||
|CMAKE_INSTALL_PREFIX|*an absolute path*|"/usr/local"|"%ProgramFiles%\dmitigr_cefeika"|
|DMITIGR_CEFEIKA_SHARE_INSTALL_DIR|*a path relative to CMAKE_INSTALL_PREFIX*|"share/dmitigr_cefeika"|"."|
|DMITIGR_CEFEIKA_CMAKE_INSTALL_DIR|*a path relative to CMAKE_INSTALL_PREFIX*|"${DMITIGR_CEFEIKA_SHARE_INSTALL_DIR}/cmake"|"cmake"|
|DMITIGR_CEFEIKA_DOC_INSTALL_DIR|*a path relative to CMAKE_INSTALL_PREFIX*|"${DMITIGR_CEFEIKA_SHARE_INSTALL_DIR}/doc"|"doc"|
|DMITIGR_CEFEIKA_LIB_INSTALL_DIR|*a path relative to CMAKE_INSTALL_PREFIX*|"lib"|"lib"|
|DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR|*a path relative to CMAKE_INSTALL_PREFIX*|"include"|"include"|
|**Options of the libev**||||
|LIBEV_PREFIX|*a path*|*not set (rely on CMake)*|*not set (rely on CMake)*|
|LIBEV_LIB_PREFIX|*a path*|${LIBEV_PREFIX}|${LIBEV_PREFIX}|
|LIBEV_INCLUDE_PREFIX|*a path*|${LIBEV_PREFIX}|${LIBEV_PREFIX}|
|**Options of the libpq**||||
|LIBPQ_PREFIX|*a path*|*not set (rely on CMake)*|*not set (rely on CMake)*|
|LIBPQ_LIB_PREFIX|*a path*|${LIBPQ_PREFIX}|${LIBPQ_PREFIX}|
|LIBPQ_INCLUDE_PREFIX|*a path*|${LIBPQ_PREFIX}|${LIBPQ_PREFIX}|
|**Options of the libuv**||||
|LIBUV_PREFIX|*a path*|*not set (rely on CMake)*|*not set (rely on CMake)*|
|LIBUV_LIB_PREFIX|*a path*|${LIBUV_PREFIX}|${LIBUV_PREFIX}|
|LIBUV_INCLUDE_PREFIX|*a path*|${LIBUV_PREFIX}|${LIBUV_PREFIX}|

### Remarks

  - `LIB<LIB>_PREFIX` specifies a prefix for both binary and headers of the
  `<LIB>`. For example, if [PostgreSQL] installed relocatably into
  `/usr/local/pgsql`, the value of `LIBPQ_PREFIX` may be set accordingly;
  - `LIBPQ_LIB_PREFIX` specifies a prefix of the [libpq] binary (shared library);
  - `LIBPQ_INCLUDE_PREFIX` specifies a prefix of the [libpq] headers (namely,
  `libpq-fe.h`).

  Note, that when building with Visual Studio on Windows the value of
  `CMAKE_BUILD_TYPE` doesn't selects the build configuration within the
  generated build environment. The [CMake] command line option `--config` may
  be used for that purpose.

  Note, on Windows [CMake] will automatically search for dependency libraries in
  `<prefix>/lib` for each `<prefix>/[s]bin` found in `PATH` environment variable,
  and `<prefix>/lib` for other entries of `PATH`, and the directories of `PATH`
  itself.

## Installation

Cefeika can be installed as a set of:

  - shared libraries if `-DBUILD_SHARED_LIBS=ON` option is specified;
  - static libraries if `-DBUILD_SHARED_LIBS=OFF` option is specified
    (the default);
  - header-only libraries if `-DDMITIGR_CEFEIKA_HEADER_ONLY=ON` option
    is specified.

The default build type is *Debug*.

### Installation on Linux

    $ git clone https://github.com/dmitigr/cefeika.git
    $ mkdir cefeika/build
    $ cd cefeika/build
    $ cmake -DCMAKE_BUILD_TYPE=Release ..
    $ cmake --build . --parallel
    $ cmake sudo make install

### Installation on Microsoft Windows

Run Developer Command Prompt for Visual Studio and type:

    > git clone https://github.com/dmitigr/cefeika.git
    > mkdir cefeika\build
    > cd cefeika\build
    > cmake -G "Visual Studio 15 2017 Win64" ..
    > cmake --build . --config Release --parallel

Next, run the elevated command prompt (i.e. the command prompt with
administrator privileges) and type:

    > cmake -DBUILD_TYPE=Release -P cmake_install.cmake

Alternatively, the following build command may be used:

    > cmake --build . --config Release --target install

**A bitness of the target architecture must corresponds to the bitness
of external dependencies!**

To make installed DLLs available for *any* application that depends on it,
symbolic links can be created:

  - in `%SYSTEMROOT%\System32` for a 64-bit DLL on a 64-bit host
    (or for the 32-bit DLL on the 32-bit host);
  - in `%SYSTEMROOT%\SysWOW64` for the 32-bit DLL on 64-bit host.

For example, to create the symbolic link to `dmitigr_pgfed.dll`, the `mklink`
command can be used in the elevated command prompt:

    > cd /d %SYSTEMROOT%\System32
    > mklink dmitigr_pgfed.dll "%ProgramFiles%\dmitigr_cefeika\lib\dmitigr_pgfed.dll"

## Usage

Assuming `foo` is the name of library, the following considerations should be
followed:

  - headers other than `dmitigr/foo.hpp` should *not* be used
    since these headers are subject to reorganize;
  - namespace `dmitigr::foo::detail` should *not* be used directly
    since it consists of the implementation details.

### Usage with CMake

With [CMake] it's pretty easy to use the libraries (including standalone versions)
in two ways: as a system-wide installed library(-es) or as a library(-es) dropped
into the project source directory.

The code below demonstrates how to import system-wide installed Cefeika libraries
by using [CMake] (this snippet is also valid when using the standalone libraries):

```cmake
cmake_minimum_required(VERSION 3.16)
project(foo)
find_package(dmitigr_cefeika REQUIRED COMPONENTS fcgi pgfe)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(foo foo.cpp)
target_link_libraries(foo dmitigr_fcgi dmitigr_pgfe)
```

The next code demonstrates how to import the standalone [Pgfe][dmitigr_pgfe]
library dropped directly into the project's source directory `3rdparty/pgfe`:

```cmake
set(DMITIGR_CEFEIKA_HEADER_ONLY ON CACHE BOOL "Header-only?")
add_subdirectory(3rdparty/pgfe)
```

Note, that all CMake variables described in [CMake options](#cmake-options) are
also valid for standalone versions of libraries.

#### Specifying a library type to use

It's possible to explicitly specify a type of library to use. To do it,
the corresponding suffix of a component name should be specified:

  - the suffix "_shared" corresponds to shared libraries;
  - the suffix "_static" corresponds to static libraries;
  - the suffix "_interface" corresponds to header-only libraries.

For example, the code below demonstrates how to use the shared [fcgi] library
and the header-only [pgfe] library in a same project side by side:

```cmake
find_package(dmitigr_cefeika REQUIRED COMPONENTS fcgi_shared pgfe_interface)
# ...
target_link_libraries(foo dmitigr_fcgi dmitigr_pgfe)
```

**Note that libraries of the explicitly specified types must be installed
to be found!**

If the type of library is not specified (i.e. suffix of a component name is
omitted), [find_package()][CMake_find_package] will try to import the first
available library in the following order:

  1. a shared library;
  2. a static library;
  3. a header-only library.

### Usage without CMake

It's possible to use the libraries without [CMake]. In order to use header-only
libraries the macros `DMITIGR_FOO_HEADER_ONLY`, where `FOO` - is a library name
in uppercase, must be defined before including a library header, for example:

```cpp
#define DMITIGR_PGFE_HEADER_ONLY
#include <dmitigr/pgfe.hpp>
// ...
```

Please note, that external dependencies  must be linked manually in this case!

## Licenses and copyrights

Cefeika itself (except the software of third parties it's includes) is
distributed under zlib [LICENSE](LICENSE.txt).

Cefeika includes the following software of third parties:

  - [RapidJSON] is distributed under the following [LICENSE](lib/dmitigr/3rdparty/rapidjson/license.txt);
  - [uSockets] is distributed under the following [LICENSE](lib/dmitigr/3rdparty/usockets/LICENSE);
  - [uWebSockets] is distributed under the following [LICENSE](lib/dmitigr/3rdparty/uwebsockets/LICENSE);
  - [libuwsc] is distributed under the following [LICENSE](lib/dmitigr/3rdparty/uwsc/LICENSE).

For conditions of distribution and use, please see the corresponding license.

[dmitigr_mail]: mailto:dmitigr@gmail.com
[dmitigr_paypal]: https://paypal.me/dmitigr
[dmitigr_cefeika]: https://github.com/dmitigr/cefeika.git
[dmitigr_fcgi]: https://github.com/dmitigr/fcgi.git
[dmitigr_pgfe]: https://github.com/dmitigr/pgfe.git

[dt]: https://github.com/dmitigr/cefeika/tree/master/dt
[fcgi]: https://github.com/dmitigr/cefeika/tree/master/fcgi
[http]: https://github.com/dmitigr/cefeika/tree/master/http
[jrpc]: https://github.com/dmitigr/cefeika/tree/master/jrpc
[net]: https://github.com/dmitigr/cefeika/tree/master/net
[os]: https://github.com/dmitigr/cefeika/tree/master/os
[pgfe]: https://github.com/dmitigr/cefeika/tree/master/pgfe
[rajson]: https://github.com/dmitigr/cefeika/tree/master/rajson
[sqlixx]: https://github.com/dmitigr/cefeika/tree/master/sqlixx
[web]: https://github.com/dmitigr/cefeika/tree/master/web
[ws]: https://github.com/dmitigr/cefeika/tree/master/ws
[wscl]: https://github.com/dmitigr/cefeika/tree/master/wscl

[CMake]: https://cmake.org/
[CMake_find_package]: https://cmake.org/cmake/help/latest/command/find_package.html
[GCC]: https://gcc.gnu.org/
[json-rpc2]: https://www.jsonrpc.org/specification
[libev]: http://software.schmorp.de/pkg/libev.html
[libpq]: https://www.postgresql.org/docs/current/static/libpq.html
[libuv]: https://libuv.org/
[OpenSSL]: https://www.openssl.org/
[PostgreSQL]: https://www.postgresql.org/
[RapidJSON]: http://rapidjson.org/
[SQLite]: https://www.sqlite.org/
[uSockets]: https://github.com/uNetworking/uSockets
[uWebSockets]: https://github.com/uNetworking/uWebSockets
[libuwsc]: https://github.com/zhaojh329/libuwsc
[Visual_Studio]: https://www.visualstudio.com/
[zlib]: https://zlib.net/
