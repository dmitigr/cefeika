// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or mulf.hpp

#include "../../filesystem.hpp"
#include "../../mulf.hpp"
#include "../../str.hpp"
#include "../../testo.hpp"

int main(int, char* argv[])
{
  namespace mulf = dmitigr::mulf;
  namespace str = dmitigr::str;
  using mulf::Form_data;
  using namespace dmitigr::testo;

  try {
    const std::filesystem::path this_exe_file_name{argv[0]};
    const auto this_exe_dir_name = this_exe_file_name.parent_path();
    const auto form_data = str::to_string(this_exe_dir_name / "mulf-form-data-valid1.txt");

    const std::string boundary{"AaB03x"};
    const Form_data data{form_data, boundary};
    ASSERT(data.entry_count() == 2);

    {
      const auto& e = data.entry(0);
      ASSERT(e.name() == "field1");
      ASSERT(!e.filename());
      ASSERT(e.content_type() == "text/plain");
      ASSERT(e.charset() == "UTF-8");
      ASSERT(e.content() == "Field1 data.");
    }

    {
      const auto& e = data.entry(1);
      ASSERT(e.name() == "field2");
      ASSERT(e.filename() == "text.txt");
      ASSERT(e.content_type() == "text/plain");
      ASSERT(e.charset() == "utf-8");
      ASSERT(e.content() == "Field2 data.");
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
