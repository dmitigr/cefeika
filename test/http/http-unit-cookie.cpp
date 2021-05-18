// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt

#include "../../http.hpp"
#include "../../testo.hpp"

int main(int, char* argv[])
{
  namespace http = dmitigr::http;
  using namespace dmitigr::testo;
  using http::Cookie;

  try {
    {
      Cookie c;
      ASSERT(c.entry_count() == 0);
      ASSERT(c.field_name() == "Cookie");
      ASSERT(!c.has_entries());

      const auto c_copy = c;
      ASSERT(c_copy.entry_count() == 0);
      ASSERT(!c_copy.has_entries());

      c.append_entry("name", "value");
      ASSERT(c.entry_count() == 1);
      ASSERT(c.entry_index("name") == 0);
      ASSERT(c.entry(0).name() == "name");
      ASSERT(c.entry(0).value() == "value");
      ASSERT(c.entry("name").name() == "name");
      ASSERT(c.entry("name").value() == "value");
      ASSERT(c.has_entry("name"));
      ASSERT(c.has_entries());

      c.remove_entry("name");
      ASSERT(c.entry_count() == 0);
      ASSERT(!c.has_entries());

      c.append_entry("name", "value");
      c.remove_entry(0);
      ASSERT(c.entry_count() == 0);
      ASSERT(!c.has_entries());

      c.append_entry("name", "value");
      c.entry(0).set_name("another_name");
      ASSERT(c.entry_index("another_name") == 0);
      ASSERT(c.entry(0).name() == "another_name");

      c.entry("another_name").set_name("name");
      ASSERT(c.entry_index("name") == 0);
      ASSERT(c.entry(0).name() == "name");

      c.entry("name").set_value("another_value");
      ASSERT(c.entry("name").value() == "another_value");
      ASSERT(c.entry(0).name() == "name");
    }

    {
      const Cookie c{"name=value"};
      ASSERT(c.entry_count() == 1);
      ASSERT(c.entry_index("name") == 0);
      ASSERT(c.entry(0).name() == "name");
      ASSERT(c.entry(0).value() == "value");
      ASSERT(c.entry("name").value() == "value");
      ASSERT(c.has_entry("name"));
      ASSERT(c.has_entries());
    }

    {
      const Cookie c{"name=value; name2=value2; name3=value3"};
      ASSERT(c.entry_count() == 3);
      ASSERT(c.has_entries());
      //
      ASSERT(c.entry_index("name") == 0);
      ASSERT(c.entry(0).name() == "name");
      ASSERT(c.entry(0).value() == "value");
      ASSERT(c.entry("name").name() == "name");
      ASSERT(c.entry("name").value() == "value");
      ASSERT(c.has_entry("name"));
      //
      ASSERT(c.entry_index("name2") == 1);
      ASSERT(c.entry(1).name() == "name2");
      ASSERT(c.entry(1).value() == "value2");
      ASSERT(c.entry("name2").name() == "name2");
      ASSERT(c.entry("name2").value() == "value2");
      ASSERT(c.has_entry("name2"));
      //
      ASSERT(c.entry_index("name3") == 2);
      ASSERT(c.entry(2).name() == "name3");
      ASSERT(c.entry(2).value() == "value3");
      ASSERT(c.entry("name3").name() == "name3");
      ASSERT(c.entry("name3").value() == "value3");
      ASSERT(c.has_entry("name3"));
    }
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 2;
  }

  return 0;
}
