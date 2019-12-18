// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#include "dmitigr/jrpc/error.hpp"
#include "dmitigr/jrpc/implementation_header.hpp"

#include "dmitigr/util/debug.hpp"

namespace dmitigr::jrpc {

DMITIGR_JRPC_INLINE Error::Error(const std::error_code code,
  rapidjson::Value id, const std::string& message)
  : system_error{std::move(code), message}
  , rep_{std::make_shared<rapidjson::Document>(rapidjson::kObjectType)}
{
  using T = rapidjson::Type;
  using V = rapidjson::Value;
  auto& alloc = allocator();
  rep_->AddMember("jsonrpc", "2.0", alloc);
  rep_->AddMember("id", id, alloc);
  {
    V e{T::kObjectType};
    e.AddMember("code", V{this->code().value()}, alloc);
    e.AddMember("message", message, alloc);
    rep_->AddMember("error", std::move(e), alloc);
  }
}

DMITIGR_JRPC_INLINE Error::Error(const std::error_code code,
  const std::string& message, std::shared_ptr<rapidjson::Document> rep)
  : system_error{std::move(code), message}
  , rep_{std::move(rep)}
{
  DMITIGR_ASSERT(rep_ != nullptr);
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

DMITIGR_JRPC_INLINE rapidjson::Value::AllocatorType& Error::allocator()
{
  return rep_->GetAllocator();
}

} // namespace dmitigr jrpc

#include "dmitigr/jrpc/implementation_footer.hpp"
