# The cross-platform C++ libraries for the backend development

Dmitigr Cefeika (hereinafter referred to as Cefeika) includes:

  - [algo] - algorithms library
  - [app] - application support library
  - [cfg] - library to work with configuration files
  - [dt] - library to work with date and time
  - [fcgi] - powerful FastCGI implementation (server)
  - [fs] - filesystem extras
  - [http] - HTTP library
  - [img] - image processing library
  - [jrpc] - [JSON-RPC 2.0][json-rpc2] implementation
  - [math] - mathematical stuff
  - [mem] - memory management utilities
  - [mulf] - multipart/form-data library
  - [net] - networking library
  - [os] - OS interaction
  - [pgfe] - powerful client API for [PostgreSQL]
  - [rajson] - [RapidJSON] wrapper
  - [rng] - random number generators
  - [str] - string extras
  - [thread] - threading stuff
  - [ttpl] - text templates library
  - [url] - URL processing
  - [util] - basic utilities
  - [uuid] - UUID processing
  - [web] - Web utilities
  - [ws] - Websockets library

All of these libraries can be used as shared libraries, static libraries or
header-only libraries. ([ws] requires to build some third-party libraries
such as [libuv] and [uSockets] which are shipped with Cefeika.)

**Most of these libraries are in *work in progress* state! Participations and
contributions of any kind are welcome!**

Some of these libraries are available as standalone versions:

  - [Dmitigr Fcgi][dmitigr_fcgi];
  - [Dmitigr Pgfe][dmitigr_pgfe].

Any feedback are [welcome][dmitigr_mail]. Donations are also [welcome][dmitigr_paypal].

## Third-party dependencies

- [CMake] build system version 3.10+;
- C++17 compiler ([GCC] 7.4+ or [Microsoft Visual C++][Visual_Studio] 15.7+).

Also:

- [libpq] library for [pgfe];
- [GraphicsMagick] library for [img].

## Third-party software which are shipped with Cefeika

|Name|Source|
|:---|:------|
|libuv|https://github.com/dmitigr/libuv/tree/v1.x|
|RapidJSON|https://github.com/dmitigr/rapidjson/tree/master|
|uSockets|https://github.com/dmitigr/uSockets/tree/master|
|uWebSockets|https://github.com/dmitigr/uWebSockets/tree/master|

## CMake options

The table below (one may need to use horizontal scrolling for full view)
contains variables which can be passed to [CMake] for customization.

|CMake variable|Possible values|Default on Unix|Default on Windows|
|:-------------|:--------------|:--------------|:-----------------|
|**The type of the build**||||
|CMAKE_BUILD_TYPE|Debug \| Release \| RelWithDebInfo \| MinSizeRel|Debug|Debug|
|**The flag to build the shared libraries**||||
|BUILD_SHARED_LIBS|On \| Off|On|On|
|**The flag to only install the header-only libraries**||||
|DMITIGR_CEFEIKA_HEADER_ONLY|On \| Off|Off|Off|
|**The flag to build the tests**||||
|DMITIGR_CEFEIKA_BUILD_TESTS|On \| Off|On|On|
|**Installation directories**||||
|CMAKE_INSTALL_PREFIX|*an absolute path*|"/usr/local"|"%ProgramFiles%\dmitigr_cefeika"|
|DMITIGR_CEFEIKA_SHARE_INSTALL_DIR|*a path relative to CMAKE_INSTALL_PREFIX*|"share/dmitigr_cefeika"|"."|
|DMITIGR_CEFEIKA_CMAKE_INSTALL_DIR|*a path relative to CMAKE_INSTALL_PREFIX*|"${DMITIGR_CEFEIKA_SHARE_INSTALL_DIR}/cmake"|"cmake"|
|DMITIGR_CEFEIKA_DOC_INSTALL_DIR|*a path relative to CMAKE_INSTALL_PREFIX*|"${DMITIGR_CEFEIKA_SHARE_INSTALL_DIR}/doc"|"doc"|
|DMITIGR_CEFEIKA_LIB_INSTALL_DIR|*a path relative to CMAKE_INSTALL_PREFIX*|"lib"|"lib"|
|DMITIGR_CEFEIKA_INCLUDE_INSTALL_DIR|*a path relative to CMAKE_INSTALL_PREFIX*|"include"|"include"|
|**Options of the Pgfe library**||||
|LIBPQ_PREFIX|*a path*|*not set (rely on CMake)*|*not set (rely on CMake)*|
|LIBPQ_LIB_PREFIX|*a path*|${LIBPQ_PREFIX}|${LIBPQ_PREFIX}|
|LIBPQ_INCLUDE_PREFIX|*a path*|${LIBPQ_PREFIX}|${LIBPQ_PREFIX}|

### Remarks

  - `LIBPQ_PREFIX` specifies a prefix for both binary and headers of [libpq].
  For example, if [PostgreSQL] installed relocatably into `/usr/local/pgsql`,
  the value of `LIBPQ_PREFIX` may be set accordingly;
  - `LIBPQ_LIB_PREFIX` specifies a prefix of the [libpq] binary (shared library);
  - `LIBPQ_INCLUDE_PREFIX` specifies a prefix of the [libpq] headers (namely,
  `libpq-fe.h`).

  Note, on Windows [CMake] will automatically search for dependency libraries in
  `<prefix>/lib` for each `<prefix>/[s]bin` found in `PATH` environment variable,
  and `<prefix>/lib` for other entries of `PATH`, and the directories of `PATH`
  itself.

## Installation

Cefeika can be installed as a set of:

  - shared libraries if `-DBUILD_SHARED_LIBS=ON` option is specified
    (by default);
  - static libraries if `-DBUILD_SHARED_LIBS=OFF` option is specified;
  - header-only libraries if `-DDMITIGR_CEFEIKA_HEADER_ONLY=ON` option
    is specified.

The default build type is *Debug*.

### Installation on Linux

    $ git clone https://github.com/dmitigr/cefeika.git
    $ mkdir cefeika/build
    $ cd cefeika/build
    $ cmake ..
    $ cmake --build . --parallel
    $ cmake sudo make install

### Installation on Microsoft Windows

Run Developer Command Prompt for Visual Studio and type:

    > git clone https://github.com/dmitigr/cefeika.git
    > mkdir cefeika\build
    > cd cefeika\build
    > cmake -G "Visual Studio 15 2017 Win64" ..
    > cmake --build . --parallel

Next, run the elevated command prompt (i.e. the command prompt with
administrator privileges) and type:

    > cd cefeika\build
    > cmake -DBUILD_TYPE=Debug -P cmake_install.cmake

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
cmake_minimum_required(VERSION 3.13)
project(foo)
find_package(dmitigr_cefeika REQUIRED COMPONENTS fcgi pgfe)
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
add_executable(foo foo.cpp)
target_link_libraries(foo dmitigr::fcgi dmitigr::pgfe)
```

The next code demonstrates how to import the standalone [Pgfe][dmitigr_pgfe]
library dropped directly into the project's source directory `third-party/pgfe`:

```cmake
set(DMITIGR_CEFEIKA_HEADER_ONLY ON CACHE BOOL "Header-only?")
add_subdirectory(third-party/pgfe)
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
target_link_libraries(foo dmitigr::fcgi dmitigr::pgfe)
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

It's highly recommended to use the helper header `cefeika_header_only.hpp`
in which `DMITIGR_FOO_HEADER_ONLY` macros are properly defined, for example:

```cpp
#include <dmitigr/cefeika_header_only.hpp>
#include <dmitigr/fcgi.hpp>
#include <dmitigr/pgfe.hpp>
// ...
```

Please note, that external dependencies  must be linked manually in this case!

## Licenses and copyrights

Cefeika itself (except the software of third parties it's includes) is
distributed under zlib [LICENSE](LICENSE.txt).

Cefeika includes the following software of third parties:

  - [RapidJSON] distributed under the following [LICENSE](thirdparty/rapidjson/license.txt);
  - [uSockets] distributed under the following [LICENSE](thirdparty/usockets/LICENSE);
  - [libuv] distributed under the following [LICENSE](thirdparty/uv/LICENSE);
  - [uWebSockets] distributed under the following [LICENSE](thirdparty/uwebsockets/LICENSE).

For conditions of distribution and use, please see the corresponding license.

[dmitigr_mail]: mailto:dmitigr@gmail.com
[dmitigr_paypal]: https://paypal.me/dmitigr
[dmitigr_cefeika]: https://github.com/dmitigr/cefeika.git
[dmitigr_fcgi]: https://github.com/dmitigr/fcgi.git
[dmitigr_pgfe]: https://github.com/dmitigr/pgfe.git

[algo]: https://github.com/dmitigr/cefeika/tree/master/lib/dmitigr/algo
[app]: https://github.com/dmitigr/cefeika/tree/master/lib/dmitigr/app
[cfg]: https://github.com/dmitigr/cefeika/tree/master/lib/dmitigr/cfg
[dt]: https://github.com/dmitigr/cefeika/tree/master/doc/dt
[fcgi]: https://github.com/dmitigr/cefeika/tree/master/doc/fcgi
[fs]: https://github.com/dmitigr/cefeika/tree/master/lib/dmitigr/fs
[http]: https://github.com/dmitigr/cefeika/tree/master/doc/http
[img]: https://github.com/dmitigr/cefeika/tree/master/doc/img
[jrpc]: https://github.com/dmitigr/cefeika/tree/master/doc/jrpc
[math]: https://github.com/dmitigr/cefeika/tree/master/lib/dmitigr/math
[mem]: https://github.com/dmitigr/cefeika/tree/master/lib/dmitigr/mem
[mulf]: https://github.com/dmitigr/cefeika/tree/master/doc/mulf
[net]: https://github.com/dmitigr/cefeika/tree/master/lib/dmitigr/net
[os]: https://github.com/dmitigr/cefeika/tree/master/lib/dmitigr/os
[pgfe]: https://github.com/dmitigr/cefeika/tree/master/doc/pgfe
[rajson]: https://github.com/dmitigr/cefeika/tree/master/doc/rajson
[rng]: https://github.com/dmitigr/cefeika/tree/master/lib/dmitigr/rng
[str]: https://github.com/dmitigr/cefeika/tree/master/lib/dmitigr/str
[thread]: https://github.com/dmitigr/cefeika/tree/master/lib/dmitigr/thread
[ttpl]: https://github.com/dmitigr/cefeika/tree/master/doc/ttpl
[url]: https://github.com/dmitigr/cefeika/tree/master/doc/url
[util]: https://github.com/dmitigr/cefeika/tree/master/doc/util
[uuid]: https://github.com/dmitigr/cefeika/tree/master/lib/dmitigr/uuid
[web]: https://github.com/dmitigr/cefeika/tree/master/lib/dmitigr/web
[ws]: https://github.com/dmitigr/cefeika/tree/master/doc/ws

[CMake]: https://cmake.org/
[CMake_find_package]: https://cmake.org/cmake/help/latest/command/find_package.html
[GCC]: https://gcc.gnu.org/
[GraphicsMagick]: http://www.graphicsmagick.org/
[json-rpc2]: https://www.jsonrpc.org/specification
[libpq]: https://www.postgresql.org/docs/current/static/libpq.html
[libuv]: https://libuv.org/
[PostgreSQL]: https://www.postgresql.org/
[RapidJSON]: http://rapidjson.org/
[uSockets]: https://github.com/uNetworking/uSockets
[uWebSockets]: https://github.com/uNetworking/uWebSockets
[Visual_Studio]: https://www.visualstudio.com/
