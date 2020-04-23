// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#include "dmitigr/jrpc/error.hpp"
#include "dmitigr/jrpc/result.hpp"
#include <dmitigr/rajson/conversions.hpp>
#include <dmitigr/base/debug.hpp>

namespace dmitigr::jrpc {

DMITIGR_JRPC_INLINE std::unique_ptr<Response> Response::make(const std::string_view input)
{
  rapidjson::Document rep{rajson::to_parsed_json(input)};
  if (rep.HasParseError())
    throw std::runtime_error{"dmitigr::jrpc: response parse error"};

  if (rep.MemberCount() != 3)
    throw std::runtime_error{"dmitigr::jrpc: invalid member count in response"};

  const auto e = rep.MemberEnd();

  // Checking jsonrpc member.
  if (const auto i = rep.FindMember("jsonrpc"); i != e) {
    if (i->value.IsString()) {
      const auto jsonrpc = rajson::to<std::string_view>(i->value);
      constexpr std::string_view ver{"2.0", 3};
      if (jsonrpc != ver)
        throw std::runtime_error{"dmitigr::jrpc: invalid value of \"jsonrpc\" member of response"};
    } else
      throw std::runtime_error{"dmitigr::jrpc: invalid type of \"jsonrpc\" member of response"};
  } else
    throw std::runtime_error{"dmitigr::jrpc: no \"jsonrpc\" member found in response"};

  // Checking id member.
  if (const auto i = rep.FindMember("id"); i != e) {
    if (!i->value.IsNumber() && !i->value.IsString() && !i->value.IsNull())
      throw std::runtime_error{"dmitigr::jrpc: invalid type of \"id\" member of response"};
  } else
    throw std::runtime_error{"dmitigr::jrpc: no \"id\" member found in response"};

  // Checking result/error member.
  const auto ri = rep.FindMember("result");
  const auto ei = rep.FindMember("error");

  if (ri != e) {
    if (ei != e)
      throw std::runtime_error{"dmitigr::jrpc: both \"result\" and \"error\" member found in response"};
    else
      return std::unique_ptr<Result>(new Result{std::move(rep)});
  } else if (ei != e) {
    if (ei->value.IsObject()) {
      std::size_t expected_error_member_count = 3;
      auto& ev = ei->value;
      const auto ee = ev.MemberEnd();

      // Checking error.code member.
      const auto codei = ev.FindMember("code");
      if (codei != ee) {
        if (!codei->value.IsInt())
          throw std::runtime_error{"dmitigr::jrpc: invalid type of \"error.code\" member"};
      } else
        throw std::runtime_error{"dmitigr::jrpc: no \"error.code\" member found"};

      // Checking error.message member.
      const auto msgi = ev.FindMember("message");
      if (msgi != ee) {
        if (!msgi->value.IsString())
          throw std::runtime_error{"dmitigr::jrpc: invalid type of \"error.message\" member"};
      } else
        throw std::runtime_error{"dmitigr::jrpc: no \"error.message\" member found"};

      // Checking error.data member.
      if (const auto i = ev.FindMember("data"); i == ee)
        expected_error_member_count--;

      // Checking member count of error member.
      if (ev.MemberCount() != expected_error_member_count)
        throw std::runtime_error{"dmitigr::jrpc: invalid member count of \"error\" member"};

      // Done.
      return std::unique_ptr<Error>{new Error{
        std::error_code{rajson::to<int>(codei->value), server_error_category()},
        rajson::to<std::string>(msgi->value),
        std::make_shared<rapidjson::Document>(std::move(rep))}};
    } else
      throw std::runtime_error{"dmitigr::jrpc: invalid type of \"error\" member"};
  } else
    throw std::runtime_error{"dmitigr::jrpc: nor \"result\" nor \"error\" member found in response"};
}

} // namespace dmitigr::jrpc
