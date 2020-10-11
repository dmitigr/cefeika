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
    ASSERT(composite.size() == 0);
    ASSERT(composite.is_empty());
    // Modifying the composite.
    ASSERT(composite.size() == 0);
    composite.append("foo", {});
    ASSERT(composite.size() == 1);
    ASSERT(!composite.is_empty());
    ASSERT(composite.name_of(0) == "foo");
    ASSERT(composite.index_of("foo") == 0);
    ASSERT(composite.data(0) == nullptr);
    ASSERT(composite.data("foo") == nullptr);
    composite.set_data("foo", "foo data");
    ASSERT(pgfe::to<std::string_view>(composite.data(0).get()) == "foo data");
    ASSERT(pgfe::to<std::string_view>(composite.data("foo").get()) == "foo data");
    //
    ASSERT(composite.size() == 1);
    composite.append("bar", "bar data");
    ASSERT(composite.size() == 2);
    ASSERT(!composite.is_empty());
    ASSERT(composite.name_of(1) == "bar");
    ASSERT(composite.index_of("bar") == 1);
    ASSERT(pgfe::to<std::string_view>(composite.data(1).get()) == "bar data");
    ASSERT(pgfe::to<std::string_view>(composite.data("bar").get()) == "bar data");
    //
    composite.insert("bar", "baz", 1983);
    ASSERT(composite.size() == 3);
    ASSERT(pgfe::to<int>(composite.data("baz").get()) == 1983);
    composite.remove("foo");
    ASSERT(composite.size() == 2);
    ASSERT(composite.index_of("foo") == pgfe::Composite::nidx);
    composite.remove("bar");
    ASSERT(composite.size() == 1);
    ASSERT(composite.index_of("bar") == pgfe::Composite::nidx);
    ASSERT(composite.index_of("baz") != pgfe::Composite::nidx);
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 1;
  }
}
