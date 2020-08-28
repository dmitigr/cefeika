// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#include "pgfe-unit-benchmark_array.hpp"

#include <dmitigr/pgfe/conversions.hpp>
#include <dmitigr/pgfe/row.hpp>
#include <dmitigr/pgfe/row_info.hpp>

#include <type_traits>

int main(int argc, char* argv[])
{
  namespace pgfe = dmitigr::pgfe;
  using namespace dmitigr::testo;

  try {
    auto [output_file, conn] = pgfe::test::arraybench::prepare(argc, argv);
    conn->perform("select dat[1], dat[2], dat[3], dat[4], dat[5] from benchmark_test_array");
    ASSERT(conn->row());
    const auto field_count = conn->row()->info().field_count();
    ASSERT(field_count == 5);
    conn->for_each([&](const pgfe::Row* const r) {
      using Counter = std::decay_t<decltype (field_count)>;
      for (Counter i = 0; i < field_count; ++i)
        output_file << pgfe::to<std::string>(r->data(0));
      output_file << "\n";
    });
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 1;
  }
}
