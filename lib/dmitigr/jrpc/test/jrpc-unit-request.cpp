// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#include "dmitigr/jrpc.hpp"

#include "dmitigr/util/test.hpp"

int main(int, char* argv[])
{
  namespace jrpc = dmitigr::jrpc;
  using namespace dmitigr::test;

  try {
    // Parse request.
    {
      const auto req = jrpc::Request::make(R"({"jsonrpc": "2.0", "method": "subtract", "params": [42, 23], "id": 1})");
      ASSERT(req->jsonrpc() == "2.0");
      ASSERT(req->method() == "subtract");
      ASSERT(req->parameters());
      ASSERT(req->parameters()->IsArray());
      ASSERT(req->has_parameters());
      ASSERT(req->parameter_count() == 2);
      ASSERT(req->id());
      ASSERT(req->id()->IsInt());
      ASSERT(req->id()->GetInt() == 1);

      ASSERT(req->parameter(0));
      ASSERT(req->parameter(0)->IsInt());
      ASSERT(req->parameter(0)->GetInt() == 42);
      ASSERT(req->parameter(1));
      ASSERT(req->parameter(1)->IsInt());
      ASSERT(req->parameter(1)->GetInt() == 23);
      ASSERT(req->to_string() == R"({"jsonrpc":"2.0","method":"subtract","params":[42,23],"id":1})");

      req->set_parameter(3, 7);
      ASSERT(req->parameter_count() == 3 + 1);
      ASSERT(req->parameter(2));
      ASSERT(req->parameter(2)->IsNull());
      ASSERT(req->parameter(3));
      ASSERT(req->parameter(3)->IsInt());
      ASSERT(req->parameter(3)->GetInt() == 7);
      ASSERT(req->to_string() == R"({"jsonrpc":"2.0","method":"subtract","params":[42,23,null,7],"id":1})");

      req->reset_parameters(jrpc::Parameters_notation::positional);
      ASSERT(req->parameters());
      ASSERT(!req->has_parameters());
      ASSERT(req->parameter_count() == 0);
      ASSERT(req->to_string() == R"({"jsonrpc":"2.0","method":"subtract","params":[],"id":1})");

      req->omit_parameters();
      ASSERT(!req->parameters());
      ASSERT(!req->has_parameters());
      ASSERT(req->parameter_count() == 0);
      ASSERT(req->to_string() == R"({"jsonrpc":"2.0","method":"subtract","id":1})");

      req->set_parameter(0, 10);
      req->set_parameter(1, 5.5);
      ASSERT(req->parameters());
      ASSERT(req->parameters()->IsArray());
      ASSERT(req->has_parameters());
      ASSERT(req->parameter_count() == 2);
      ASSERT(req->parameter(0));
      ASSERT(req->parameter(0)->IsInt());
      ASSERT(req->parameter(0)->GetInt() == 10);
      ASSERT(req->parameter(1));
      ASSERT(req->parameter(1)->IsFloat());
      ASSERT(req->parameter(1)->GetFloat() == 5.5);
      ASSERT(req->to_string() == R"({"jsonrpc":"2.0","method":"subtract","id":1,"params":[10,5.5]})");
    }

    // Parse notification.
    {
      const auto req = jrpc::Request::make(R"({"jsonrpc": "2.0", "method": "update", "params": [1,2,3,4,5]})");
      ASSERT(req->jsonrpc() == "2.0");
      ASSERT(req->method() == "update");
      ASSERT(req->parameters());
      ASSERT(req->parameters()->IsArray());
      ASSERT(req->has_parameters());
      ASSERT(req->parameter_count() == 5);
      ASSERT(!req->id());
    }

    // Parse invalid request 1.
    {
      const auto f = []{
        const auto req = jrpc::Request::make(R"({"jsonrpc": "2.1", "method": "subtract", "params": [42, 23], "id": 1})");
      };
      ASSERT(is_runtime_throw_works(f));
      try {
        f();
      } catch (const jrpc::Error& e) {
        ASSERT(e.jsonrpc() == "2.0");
        ASSERT(e.id().IsInt());
        ASSERT(e.id().GetInt() == 1);
        ASSERT(e.code() == jrpc::Server_errc::invalid_request);
        ASSERT(!e.data());
      }
    }

    // Parse invalid request 2.
    {
      const auto f = []{
        const auto req = jrpc::Request::make(R"({"excess": 0, "jsonrpc": "2.0", "method": "subtract", "params": [2, 1], "id": 1})");
      };
      ASSERT(is_runtime_throw_works(f));
      try {
        f();
      } catch (const jrpc::Error& e) {
        ASSERT(e.jsonrpc() == "2.0");
        ASSERT(e.id().IsInt());
        ASSERT(e.id().GetInt() == 1);
        ASSERT(e.code() == jrpc::Server_errc::invalid_request);
        ASSERT(!e.data());
      }
    }

    // Parse invalid notification.
    {
      const auto f = []{
        const auto req = jrpc::Request::make(R"({"jsonrpc": "2.0", "METHOD": "subtract", "params": [42, 23]})");
      };
      ASSERT(is_runtime_throw_works(f));
      try {
        f();
      } catch (const jrpc::Error& e) {
        ASSERT(e.jsonrpc() == "2.0");
        ASSERT(e.id().IsNull());
        ASSERT(e.code() == jrpc::Server_errc::invalid_request);
        ASSERT(!e.data());
      }
    }

    // Making request with Null ID and empty (not omitted) params.
    {
      const auto req = jrpc::Request::make(std::nullopt, "foo");
      ASSERT(req->jsonrpc() == "2.0");
      ASSERT(req->method() == "foo");
      ASSERT(!req->parameters());
      ASSERT(req->id());
      ASSERT(req->id()->IsNull());
      req->reset_parameters(jrpc::Parameters_notation::named);
      ASSERT(req->parameters());
      ASSERT(req->parameters()->IsObject());
      ASSERT(req->parameter_count() == 0);
    }

    // Making request with int ID.
    {
      const auto req = jrpc::Request::make(3, "bar");
      ASSERT(req->jsonrpc() == "2.0");
      ASSERT(req->method() == "bar");
      ASSERT(!req->parameters());
      ASSERT(req->id());
      ASSERT(req->id()->IsInt());
      ASSERT(req->id()->GetInt() == 3);
    }

    // Making request with string ID.
    {
      const auto req = jrpc::Request::make("Id123", "baz");
      ASSERT(req->jsonrpc() == "2.0");
      ASSERT(req->method() == "baz");
      ASSERT(!req->parameters());
      ASSERT(req->id());
      ASSERT(req->id()->IsString());
      ASSERT(std::strcmp(req->id()->GetString(), "Id123") == 0);
    }

    // Making notification.
    {
      const auto req = jrpc::Request::make_notification("move");
      ASSERT(req->jsonrpc() == "2.0");
      ASSERT(req->method() == "move");
      ASSERT(!req->parameters());
      ASSERT(!req->id());

      req->set_parameter("x", 10);
      req->set_parameter("y", 20);
      ASSERT(req->parameters());
      ASSERT(req->parameters()->IsObject());
      ASSERT(req->parameter_count() == 2);
      ASSERT(req->parameter("x") && req->parameter("x")->IsInt() && req->parameter("x")->GetInt() == 10);
      ASSERT(req->parameter("y") && req->parameter("y")->IsInt() && req->parameter("y")->GetInt() == 20);
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
