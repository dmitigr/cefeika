// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or mulf.hpp

#include "unit.hpp"

#include <dmitigr/util/fs.hpp>
#include <dmitigr/mulf.hpp>

int main(int, char* argv[])
{
  namespace mulf = dmitigr::mulf;
  namespace fs = dmitigr::fs;
  using mulf::Form_data;
  using namespace dmitigr::test;

  try {
    const std::filesystem::path this_exe_file_name{argv[0]};
    const auto this_exe_dir_name = this_exe_file_name.parent_path();
    const auto form_data = fs::read_to_string(this_exe_dir_name / "mulf-form-data-valid1.txt");

    const std::string boundary{"AaB03x"};
    auto data = Form_data::make(form_data, boundary);
    ASSERT(data->entry_count() == 2);

    const auto* entry = data->entry(0);
    ASSERT(entry);
    ASSERT(entry->name() == "field1");
    ASSERT(!entry->filename());
    ASSERT(entry->content_type() == "text/plain");
    ASSERT(entry->charset() == "UTF-8");
    ASSERT(entry->content() == "Field1 data.");
    entry = data->entry(1);
    ASSERT(entry);
    ASSERT(entry->name() == "field2");
    ASSERT(entry->filename() == "text.txt");
    ASSERT(entry->content_type() == "text/plain");
    ASSERT(entry->charset() == "utf-8");
    ASSERT(entry->content() == "Field2 data.");
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 2;
  }

  return 0;
}
