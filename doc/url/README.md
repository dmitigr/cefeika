URL library in C++ {#mainpage}
==============================

Dmitigr Url (hereinafter referred to as Url) - is an easy-to-use library
to work with URLs in C++. Url is a part of the
[Dmitigr Cefeika][dmitigr_cefeika] project.

**ATTENTION, this software is "alpha" quality, and the API is a subject to change!**

Documentation
=============

The [Doxygen]-generated documentation is located [here][dmitigr_url_doc].

Example
=======

```cpp
std::string query_string(); // defined somewhere

void example()
{
  namespace url = dmitigr::url;
  auto qs = url::Query_string::make(query_string());
  std::cout << "Parameters count = " << qs->parameter_count() << "\n";
  // etc.
}
```

Features
========

Straightforward and efficient parsing of URL [query strings][Query_string].

Copyright
=========

Copyright (C) [Dmitry Igrishin][dmitigr_mail]

[dmitigr_mail]: mailto:dmitigr@gmail.com
[dmitigr_cefeika]: https://github.com/dmitigr/cefeika.git
[dmitigr_url_doc]: http://dmitigr.ru/en/projects/cefeika/url/doc/

[Doxygen]: http://doxygen.org/
[Query_string]: https://en.wikipedia.org/wiki/Query_string
