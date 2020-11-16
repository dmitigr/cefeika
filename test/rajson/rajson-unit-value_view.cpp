// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt

#include <dmitigr/misc/filesystem.hpp>
#include <dmitigr/misc/read.hpp>
#include <dmitigr/misc/testo.hpp>
#include <dmitigr/rajson.hpp>

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
  namespace read = dmitigr::read;
  using namespace dmitigr::testo;

  try {
    const std::filesystem::path this_exe_file_name{argv[0]};
    const auto this_exe_dir_name = this_exe_file_name.parent_path();
    const auto input = read::file_to_string(this_exe_dir_name / "rajson-unit-value_view.json");
    auto document = rajson::to_document(input);

    {
      const auto& constant_document = document;
      rajson::Value_view json{constant_document};
      const auto host = json.mandatory<std::string>("host");
      ASSERT(host == "localhost");
      const auto port = json.mandatory<int>("port");
      ASSERT(port == 9001);
      const auto db = json.mandatory<Db_params>("db");
      ASSERT(db.hostname == "localhost");
      ASSERT(db.port == 5432);
      ASSERT(db.database == "postgres");
      //
      {
        const auto dbv = json.mandatory("db");
        const auto hostname = dbv.mandatory<std::string>("hostname");
        const auto portnum = dbv.mandatory<int>("port");
        const auto database = dbv.mandatory<std::string>("database");
        ASSERT(hostname == "localhost");
        ASSERT(portnum == 5432);
        ASSERT(database == "postgres");
      }
    }

    {
      rajson::Value_view json{document};
      json.mandatory("host").value() = "localhost.local";
    }

    std::cout << rajson::to_stringified(document) << std::endl;
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 2;
  }
  return 0;
}
