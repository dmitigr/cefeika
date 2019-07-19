Date and time library in C++ {#mainpage}
========================================

Dmitigr Dt (hereinafter referred to as Dt) - is an easy-to-use library to work
with date and time in C++. Dt is a part of the [Dmitigr Cefeika][dmitigr_cefeika]
project.

**ATTENTION, this software is "alpha" quality, and the API is a subject to change!**

Documentation
=============

The [Doxygen]-generated documentation is located [here][dmitigr_dt_doc].

Example
=======

```cpp
#include <cassert>
#include <dmitigr/dt.hpp>

int main()
{
  namespace dt = dmitigr::dt;
  auto ts = dt::Timestamp::make();
  ts->set_date(2019, dt::Month::feb, 20);
  assert(ts->day_of_week() == dt::Day_of_week::wed);
}
```

Features
========

Parsing of HTTP-date defined in [RFC7231][rfc7231_7111].

Copyright
=========

Copyright (C) [Dmitry Igrishin][dmitigr_mail]

[dmitigr_mail]: mailto:dmitigr@gmail.com
[dmitigr_cefeika]: https://github.com/dmitigr/cefeika.git
[dmitigr_dt_doc]: http://dmitigr.ru/en/projects/cefeika/dt/doc/

[Doxygen]: http://doxygen.org/
[rfc7231_7111]: https://tools.ietf.org/html/rfc7231#section-7.1.1.1
