// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#include "dmitigr/jrpc/result.hpp"
#include <dmitigr/base/debug.hpp>

namespace dmitigr::jrpc {

DMITIGR_JRPC_INLINE Result::Result()
  : Result{rapidjson::Value{}}
{}

DMITIGR_JRPC_INLINE Result::Result(const int id)
  : Result{rapidjson::Value{id}}
{}

DMITIGR_JRPC_INLINE Result::Result(const std::string_view id)
{
  Result tmp{rapidjson::Value{id.data(), id.size(), allocator()}};
  swap(tmp);
}

DMITIGR_JRPC_INLINE void Result::swap(Result& other)
{
  rep_.Swap(other.rep_);
}

DMITIGR_JRPC_INLINE std::string_view Result::jsonrpc() const
{
  return std::string_view{"2.0", 3};
}

DMITIGR_JRPC_INLINE const rapidjson::Value& Result::id() const
{
  const auto i = rep_.FindMember("id");
  DMITIGR_ASSERT(i != rep_.MemberEnd());
  return i->value;
}

DMITIGR_JRPC_INLINE std::string Result::to_string() const
{
  return rajson::to_stringified_json(rep_);
}

DMITIGR_JRPC_INLINE rapidjson::Value::AllocatorType& Result::allocator() const
{
  return rep_.GetAllocator();
}

DMITIGR_JRPC_INLINE const rapidjson::Value& Result::data() const
{
  const auto i = rep_.FindMember("result");
  DMITIGR_ASSERT(i != rep_.MemberEnd());
  return i->value;
}

DMITIGR_JRPC_INLINE void Result::set_data(rapidjson::Value value)
{
  data__() = std::move(value);
}

// -----------------------------------------------------------------------------
// Private
// -----------------------------------------------------------------------------

DMITIGR_JRPC_INLINE Result::Result(rapidjson::Value&& id)
{
  auto& alloc = allocator();
  rep_.AddMember("jsonrpc", "2.0", alloc);
  rep_.AddMember("result", rapidjson::Value{}, alloc);
  rep_.AddMember("id", std::move(id), alloc);

  DMITIGR_ASSERT(is_invariant_ok());
}

DMITIGR_JRPC_INLINE Result::Result(const rapidjson::Value& id)
{
  Result tmp{rapidjson::Value{id, allocator()}};
  swap(tmp);
}

DMITIGR_JRPC_INLINE Result::Result(rapidjson::Document&& rep)
  : rep_{std::move(rep)}
{
  DMITIGR_ASSERT(is_invariant_ok());
}

DMITIGR_JRPC_INLINE rapidjson::Value& Result::data__()
{
  return const_cast<rapidjson::Value&>(static_cast<const Result*>(this)->data());
}

DMITIGR_JRPC_INLINE bool Result::is_invariant_ok() const
{
  const auto mc = rep_.MemberCount();
  const auto e = rep_.MemberEnd();
  const auto ji = rep_.FindMember("jsonrpc");
  const auto ri = rep_.FindMember("result");
  const auto ii = rep_.FindMember("id");
  return ji != e && ri != e && ii != e &&
    (rajson::to<std::string_view>(ji->value) == std::string_view{"2.0", 3}) &&
    (ii->value.IsInt() || ii->value.IsString() || ii->value.IsNull()) &&
    (mc == 3);
}

} // namespace dmitigr::jrpc
