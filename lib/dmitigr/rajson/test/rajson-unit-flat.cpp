// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rajson.hpp

#include <dmitigr/rajson.hpp>
#include <dmitigr/util/fs.hpp>
#include <dmitigr/util/test.hpp>

#include <iostream>

struct Db_params final {
  std::string hostname;
  int port;
  std::string database;
};

namespace dmitigr::rajson {
template<> struct Conversions<Db_params> final {
  template<class Encoding, class Allocator>
  static auto from(const rapidjson::GenericValue<Encoding, Allocator>& value)
  {
    Db_params result;
    result.hostname = value.FindMember("hostname")->value.GetString();
    result.port = value.FindMember("port")->value.GetInt();
    result.database = value.FindMember("database")->value.GetString();
    return result;
  }
};
} // namespace dmitigr::rajson

int main(int, char* argv[])
{
  namespace rajson = dmitigr::rajson;
  namespace fs = dmitigr::fs;
  using namespace dmitigr::test;

  try {
    const std::filesystem::path this_exe_file_name{argv[0]};
    const auto this_exe_dir_name = this_exe_file_name.parent_path();
    const auto input = fs::file_data_to_string(this_exe_dir_name / "rajson-unit-flat.json");
    rajson::Flat json{input};
    const auto host = json.mandatory<std::string>("host");
    ASSERT(host == "localhost");
    const auto port = json.mandatory<int>("port");
    ASSERT(port == 9001);
    const auto db = json.mandatory<Db_params>("db");
    ASSERT(db.hostname == "localhost");
    ASSERT(db.port == 5432);
    ASSERT(db.database == "postgres");
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 2;
  }
  return 0;
}
