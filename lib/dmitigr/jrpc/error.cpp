// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#include "dmitigr/jrpc/error.hpp"
#include <dmitigr/base/debug.hpp>

namespace dmitigr::jrpc {

DMITIGR_JRPC_API Error::Error()
  : Error{Server_errc::generic_error, rapidjson::Value{}}
{}

DMITIGR_JRPC_INLINE Error::Error(const std::error_code code,
  const Null, const std::string& message)
  : Error{code, rapidjson::Value{}, message}
{}

DMITIGR_JRPC_INLINE Error::Error(const std::error_code code,
  const int id, const std::string& message)
  : Error{code, rapidjson::Value{id}, message}
{}

DMITIGR_JRPC_INLINE Error::Error(const std::error_code code,
  const std::string_view id, const std::string& message)
  : Error{code, rapidjson::Value{id.data(), id.size(), allocator()}, message}
{}

DMITIGR_JRPC_INLINE void Error::swap(Error& other)
{
  rep_.swap(other.rep_);
}

DMITIGR_JRPC_INLINE std::string_view Error::jsonrpc() const
{
  return std::string_view{"2.0", 3};
}

DMITIGR_JRPC_INLINE const rapidjson::Value& Error::id() const
{
  const auto i = rep_->FindMember("id");
  DMITIGR_ASSERT(i != rep_->MemberEnd());
  return i->value;
}

DMITIGR_JRPC_INLINE std::string Error::to_string() const
{
  return rajson::to_stringified_json(*rep_);
}

DMITIGR_JRPC_INLINE const rapidjson::Value* Error::data() const
{
  const auto& e = error();
  const auto i = e.FindMember("data");
  return i != e.MemberEnd() ? &i->value : nullptr;
}

DMITIGR_JRPC_INLINE void Error::set_data(rapidjson::Value value)
{
  auto& err = error();
  if (const auto i = err.FindMember("data"); i == err.MemberEnd())
    err.AddMember("data", std::move(value), allocator());
  else
    i->value = std::move(value);
}

DMITIGR_JRPC_INLINE const rapidjson::Value& Error::error() const
{
  const auto i = rep_->FindMember("error");
  DMITIGR_ASSERT(i != rep_->MemberEnd());
  return i->value;
}

DMITIGR_JRPC_INLINE rapidjson::Value& Error::error()
{
  return const_cast<rapidjson::Value&>(static_cast<const Error*>(this)->error());
}

DMITIGR_JRPC_INLINE rapidjson::Value::AllocatorType& Error::allocator() const
{
  return rep_->GetAllocator();
}

// -----------------------------------------------------------------------------
// Private
// -----------------------------------------------------------------------------

DMITIGR_JRPC_INLINE Error::Error(std::error_code code,
  const std::string& message, std::shared_ptr<rapidjson::Document> rep)
  : system_error{std::move(code), message}
  , rep_{std::move(rep)}
{
  DMITIGR_ASSERT(rep_ != nullptr);
}

DMITIGR_JRPC_INLINE Error::Error(std::error_code code, const std::string& message)
  : Error{std::move(code), message, std::make_shared<rapidjson::Document>(rapidjson::kObjectType)}
{}

DMITIGR_JRPC_INLINE Error::Error(std::error_code code,
  rapidjson::Value&& id, const std::string& message)
  : Error{std::move(code), message}
{
  init__(std::move(id), message);
}

DMITIGR_JRPC_INLINE Error::Error(const std::error_code code,
  const rapidjson::Value& id, const std::string& message)
  : Error{std::move(code), message}
{
  init__(rapidjson::Value{id, allocator()}, message);
}

DMITIGR_JRPC_INLINE void Error::init__(rapidjson::Value&& id, const std::string& message)
{
  using T = rapidjson::Type;
  using V = rapidjson::Value;
  auto& alloc = allocator();
  rep_->AddMember("jsonrpc", "2.0", alloc);
  rep_->AddMember("id", std::move(id), alloc);
  {
    V e{T::kObjectType};
    e.AddMember("code", V{code().value()}, alloc);
    e.AddMember("message", message, alloc);
    rep_->AddMember("error", std::move(e), alloc);
  }
}

} // namespace dmitigr jrpc
