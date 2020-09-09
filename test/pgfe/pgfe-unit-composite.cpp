// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#include <dmitigr/pgfe/composite.hpp>
#include <dmitigr/pgfe/data.hpp>
#include <dmitigr/testo.hpp>

int main(int, char* argv[])
{
  namespace pgfe = dmitigr::pgfe;
  using namespace dmitigr::testo;

  try {
    pgfe::Composite composite;
    ASSERT(composite.field_count() == 0);
    ASSERT(!composite.has_fields());
    // Modifying the composite.
    ASSERT(composite.field_count() == 0);
    composite.append("foo", {});
    ASSERT(composite.field_count() == 1);
    ASSERT(composite.has_fields());
    ASSERT(composite.field_name(0) == "foo");
    ASSERT(composite.field_index("foo") == 0);
    ASSERT(composite.data(0) == nullptr);
    ASSERT(composite.data("foo") == nullptr);
    composite.set_data("foo", "foo data");
    ASSERT(pgfe::to<std::string_view>(composite.data(0).get()) == "foo data");
    ASSERT(pgfe::to<std::string_view>(composite.data("foo").get()) == "foo data");
    //
    ASSERT(composite.field_count() == 1);
    composite.append("bar", "bar data");
    ASSERT(composite.field_count() == 2);
    ASSERT(composite.has_fields());
    ASSERT(composite.field_name(1) == "bar");
    ASSERT(composite.field_index("bar") == 1);
    ASSERT(pgfe::to<std::string_view>(composite.data(1).get()) == "bar data");
    ASSERT(pgfe::to<std::string_view>(composite.data("bar").get()) == "bar data");
    //
    composite.insert("bar", "baz", 1983);
    ASSERT(composite.field_count() == 3);
    ASSERT(pgfe::to<int>(composite.data("baz").get()) == 1983);
    composite.remove("foo");
    ASSERT(composite.field_count() == 2);
    ASSERT(!composite.has_field("foo"));
    composite.remove("bar");
    ASSERT(composite.field_count() == 1);
    ASSERT(!composite.has_field("bar"));
    ASSERT(composite.has_field("baz"));
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 1;
  }
}
