The cross-platform C++ libraries for the backend development
============================================================

Dmitigr Cefeika (hereinafter referred to as Cefeika) includes:

  - [dt] - a library to work with date and time;
  - [fcgi] - a FastCGI implementation (only server at now);
  - [http] - a library to work with HTTP (no client nor server now);
  - [mulf] - a library to work with multipart/form-data;
  - [pgfe] - a client API for [PostgreSQL];
  - [ttpl] - a library to work with text templates;
  - [url] - a library to work with URL;
  - [util] - a library to support the needs of Cefeika libraries, but may be
    useful for other projects as well.

All of these libraries can be used as shared libraries, static libraries or
header-only libraries.

Dependencies
============

- [CMake] build system version 3.10+;
- C++17 compiler ([GCC] 7.4+ or [Microsoft Visual C++][Visual_Studio] 15.7+).

Also:

- [libpq] library for [pgfe];
- [GraphicsMagick] library for [util] (optional).

Customization
=============

The table below (may need to use horizontal scrolling for full view) contains
variables which can be passed to [CMake] for customization of all Cefeika
libraries. (The customization variables specific to concrete libraries are
specified in their documentation.)

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
|**Dependencies**||||
|LIBPQ_PREFIX|*a path*|*not set (rely on CMake)*|*not set (rely on CMake)*|
|LIBPQ_LIB_PREFIX|*a path*|${LIBPQ_PREFIX}|${LIBPQ_PREFIX}|
|LIBPQ_INCLUDE_PREFIX|*a path*|${LIBPQ_PREFIX}|${LIBPQ_PREFIX}|

Remarks
-------

  - `LIBPQ_PREFIX` can be used to speficy a prefix for both binary and headers
    of [libpq]. For example, if [PostgreSQL] installed relocatably into
    `/usr/local/pgsql`, the value of `LIBPQ_PREFIX` may be set accordingly at
    command line such as: `-DLIBPQ_PREFIX=/usr/local/pgsql`;
  - `LIBPQ_LIB_PREFIX` can be used to specify a *prefix* of the [libpq]
    binary file (shared library);
  - `LIBPQ_INCLUDE_PREFIX` can be used to specify a *prefix* of the [libpq]
    headers (namely, `libpq-fe.h`).

Installation
============

Cefeika can be installed as a set of:

  - shared libraries if `-DBUILD_SHARED_LIBS=ON` option is specified
    (it's by default);
  - static libraries if `-DBUILD_SHARED_LIBS=OFF` option is specified;
  - header-only libraries if `-DDMITIGR_CEFEIKA_HEADER_ONLY=ON` option
    is specified.

The default build type is "Debug".

Installation on Linux
---------------------

    $ git clone https://github.com/dmitigr/cefeika.git
    $ mkdir cefeika/build
    $ cd cefeika/build
    $ cmake ..
    $ cmake --build . --parallel
    $ sudo make install

Installation on Microsoft Windows
---------------------------------

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

Usage
=====

The code below demonstrates how to import Cefeika libraries by using [CMake]:

```cmake
cmake_minimum_required(VERSION 3.13)
project(foo)
find_package(dmitigr_cefeika REQUIRED COMPONENTS fcgi pgfe)
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
add_executable(foo foo.cpp)
target_link_libraries(foo dmitigr::fcgi dmitigr::pgfe)
```

Specifying a library type to use
--------------------------------

It's possible to explicitly specify a type of library to use. To do it,
the corresponding suffix of a component name should be specified:

  - the suffix "_shared" corresponds to shared libraries;
  - the suffix "_static" corresponds to static libraries;
  - the suffix "_interface" corresponds to header-only libraries.

For example, the code below demonstrates how to use the shared [fcgi] library
and the header-only [pgfe] library in a project side by side:

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

Usage without CMake
-------------------

Of course, it's possible to use the libraries without [CMake]. In order to use
header-only libraries the macros `DMITIGR_LIB_HEADER_ONLY`, where `LIB` - is a
library name in uppercase, must be defined before including a library header,
for example:

```cpp
#define DMITIGR_PGFE_HEADER_ONLY
#include <dmitigr/pgfe.hpp>
// ...
```

It's highly recommended to use the helper header `cefeika_header_only.hpp`
in which `DMITIGR_LIB_HEADER_ONLY` macros are already defined, for example:

```cpp
#include <dmitigr/cefeika_header_only.hpp>
#include <dmitigr/fcgi.hpp>
#include <dmitigr/pgfe.hpp>
// ...
```

Please note, that external dependencies, such as `Ws2_32.lib` on Microsoft
Windows, must be linked manually in this case!

Remarks
-------

Assuming `lib` is the name of library, the following considerations should be
followed:

  - headers other than `dmitigr/lib.hpp` should *not* be used
    since that headers are subject to reorganize;
  - the namespace `dmitigr::lib::detail` should *not* be used
    since it consists of the implementation details.

License
=======

Cefeika is distributed under zlib license. For conditions of distribution and
use, see file `LICENSE.txt`.

Participation
=============

Contributions and any feedback are highly appreciated! Donations are
[welcome][dmitigr_paypal]!

Copyright
=========

Copyright (C) [Dmitry Igrishin][dmitigr_mail]

[dmitigr_mail]: mailto:dmitigr@gmail.com
[dmitigr_paypal]: https://paypal.me/dmitigr
[dmitigr_cefeika]: https://github.com/dmitigr/cefeika.git

[dt]: https://github.com/dmitigr/cefeika/tree/master/doc/dt
[fcgi]: https://github.com/dmitigr/cefeika/tree/master/doc/fcgi
[http]: https://github.com/dmitigr/cefeika/tree/master/doc/http
[mulf]: https://github.com/dmitigr/cefeika/tree/master/doc/mulf
[pgfe]: https://github.com/dmitigr/cefeika/tree/master/doc/pgfe
[ttpl]: https://github.com/dmitigr/cefeika/tree/master/doc/ttpl
[url]: https://github.com/dmitigr/cefeika/tree/master/doc/url
[util]: https://github.com/dmitigr/cefeika/tree/master/doc/util

[CMake]: https://cmake.org/
[CMake_find_package]: https://cmake.org/cmake/help/latest/command/find_package.html
[GCC]: https://gcc.gnu.org/
[GraphicsMagick]: http://www.graphicsmagick.org/
[libpq]: https://www.postgresql.org/docs/current/static/libpq.html
[PostgreSQL]: https://www.postgresql.org/
[Visual_Studio]: https://www.visualstudio.com/
