Text templates library in C++ {#mainpage}
=========================================

Dmitigr Ttpl (hereinafter referred to as Ttpl) - is a simple library
to work with text templates in C++. Ttpl is a part of the
[Dmitigr Cefeika][dmitigr_cefeika] project.

**ATTENTION, this software is "alpha" quality, and the API is a subject to change!**

Documentation
=============

The [Doxygen]-generated documentation is located [here][dmitigr_ttpl_doc].

Example
=======

```cpp
void example()
{
  namespace ttpl = dmitigr::ttpl;
  const std::string input{"Hello {{ name }}!"};
  const auto t = ttpl::Logic_less_template::make(input);
  t->parameter("name")->set_value("Dima");
  assert(t->to_string() == input);
  assert(t->to_output() == "Hello Dima!");
}
```

Features
========

Straightforward and efficient parsing of text templates.

Copyright
=========

Copyright (C) [Dmitry Igrishin][dmitigr_mail]

[dmitigr_mail]: mailto:dmitigr@gmail.com
[dmitigr_cefeika]: https://github.com/dmitigr/cefeika.git
[dmitigr_ttpl_doc]: http://dmitigr.ru/en/projects/cefeika/ttpl/doc/

[Doxygen]: http://doxygen.org/
