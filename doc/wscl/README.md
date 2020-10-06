# Asynchronous WebSocket client in C++

Dmitigr Wscl (hereinafter referred to as Wscl) - is an asynchronous
WebSocket client in C++. (It's just wraps modified [libuwsc] at the moment.)

## Features

- asynchronous (by using [libev] at the moment);
- easy to use;
- supports SSL.

## Third-party dependencies

- [CMake] build system version 3.13+;
- C++17 compiler (such as [GCC]);
- [libev] high-performance event loop;
- [OpenSSL] (optionally) SSL implementation.

## Third-party software bundled

|Name|Source|
|:---|:------|
|buffer|https://github.com/dmitigr/buffer|
|libuwsc|https://github.com/dmitigr/libuwsc|

## Documentation

The [Doxygen]-generated documentation will be available soon.

## Hello, World

```cpp
// Not available yet.
```

[CMake]: https://cmake.org/
[Doxygen]: http://doxygen.org/
[GCC]: https://gcc.gnu.org/
[buffer]: https://github.com/dmitigr/buffer
[libev]: http://software.schmorp.de/pkg/libev.html
[libuwsc]: https://github.com/dmitigr/libuwsc
[OpenSSL]: https://www.openssl.org/
