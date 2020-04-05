// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#include <dmitigr/base/test.hpp>
#include <dmitigr/jrpc.hpp>

int main(int, char* argv[])
{
  namespace jrpc = dmitigr::jrpc;
  using namespace dmitigr::test;

  try {
    // Parse result response.
    {
      const auto res = jrpc::Response::make(R"({"jsonrpc": "2.0", "result": 19, "id": 1})");
      auto* const r = dynamic_cast<jrpc::Result*>(res.get());
      ASSERT(r);
      ASSERT(r->jsonrpc() == "2.0");
      ASSERT(r->id().IsInt());
      ASSERT(r->id().GetInt() == 1);
      ASSERT(r->data().IsInt());
      ASSERT(r->data().GetInt() == 19);
      ASSERT(r->to_string() == R"({"jsonrpc":"2.0","result":19,"id":1})");
    }

    // Parse error response.
    {
      const auto res = jrpc::Response::make(
        R"({"jsonrpc": "2.0", "error": {"code": -32601, "message": "Method not found"}, "id": "1"})");
      auto* const r = dynamic_cast<jrpc::Error*>(res.get());
      ASSERT(r);
      ASSERT(r->jsonrpc() == "2.0");
      ASSERT(r->id().IsString());
      ASSERT(std::strcmp(r->id().GetString(), "1") == 0);
      ASSERT(r->code() == jrpc::Server_errc::method_not_found);
      ASSERT(!r->data());
      ASSERT(r->to_string() == R"({"jsonrpc":"2.0","error":{"code":-32601,"message":"Method not found"},"id":"1"})");
    }

    // Making result with null id.
    {
      auto res = jrpc::Result::make(std::nullopt);
      ASSERT(res);
      ASSERT(res->jsonrpc() == "2.0");
      ASSERT(res->id().IsNull());
      ASSERT(res->data().IsNull());
      res->set_data(123);
      ASSERT(res->data().IsInt());
      ASSERT(res->data().GetInt() == 123);
    }

    // Making result with int id.
    {
      const auto res = jrpc::Result::make(1);
      ASSERT(res);
      ASSERT(res->jsonrpc() == "2.0");
      ASSERT(res->id().IsInt());
      ASSERT(res->id().GetInt() == 1);
      ASSERT(res->data().IsNull());
    }

    // Making result with string id.
    {
      const auto res = jrpc::Result::make("id123");
      ASSERT(res);
      ASSERT(res->jsonrpc() == "2.0");
      ASSERT(res->id().IsString());
      ASSERT(std::strcmp(res->id().GetString(), "id123") == 0);
      ASSERT(res->data().IsNull());
    }

    // Making error.
    {
      jrpc::Error err{jrpc::Server_errc::parse_error, rapidjson::Value{}};
      ASSERT(err.jsonrpc() == "2.0");
      ASSERT(err.id().IsNull());
      ASSERT(!err.data());
      ASSERT(err.code() == jrpc::Server_errc::parse_error);
      err.set_data("important!");
      ASSERT(err.data());
      ASSERT(err.data()->IsString());
      ASSERT(std::strcmp(err.data()->GetString(), "important!") == 0);
      ASSERT(err.to_string() == R"({"jsonrpc":"2.0","id":null,"error":{"code":-32700,"message":"","data":"important!"}})");
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
