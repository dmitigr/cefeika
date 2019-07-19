multipart/form-data library in C++ {#mainpage}
==============================================

Dmitigr Mulf (hereinafter referred to as Mulf) - is an easy-to-use library
to work with multipart/form-data in C++. Mulf is a part of the
[Dmitigr Cefeika][dmitigr_cefeika] project.

**ATTENTION, this software is "alpha" quality, and the API is a subject to change!**

Documentation
=============

The [Doxygen]-generated documentation is located [here][dmitigr_mulf_doc].

Example
=======

```cpp
std::string form_boundary(); // defined somewhere
std::string form_data(); // defined somewhere

void example()
{
  namespace mulf = dmitigr::mulf;
  auto data = mulf::Form_data::make(form_boundary(), form_data());
  std::cout << "Body parts count = " << data->body_parts_count() << "\n";
  // etc.
}
```

Features
========

Straightforward and efficient parsing of a multipart/form-data body.

Copyright
=========

Copyright (C) [Dmitry Igrishin][dmitigr_mail]

[dmitigr_mail]: mailto:dmitigr@gmail.com
[dmitigr_cefeika]: https://github.com/dmitigr/cefeika.git
[dmitigr_mulf_doc]: http://dmitigr.ru/en/projects/cefeika/mulf/doc/

[Doxygen]: http://doxygen.org/
