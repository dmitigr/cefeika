// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#include "dmitigr/jrpc/error.hpp"
#include <dmitigr/base/debug.hpp>

namespace dmitigr::jrpc {

DMITIGR_JRPC_INLINE Error::Error()
  : Error{Server_errc::generic_error, std::string{}}
{
  init__(rapidjson::Value{}, std::string{});
  DMITIGR_ASSERT(is_invariant_ok());
}

DMITIGR_JRPC_INLINE Error::Error(const std::error_code code,
  const Null, const std::string& message)
  : Error{code, message}
{
  init__(rapidjson::Value{}, message);
  DMITIGR_ASSERT(is_invariant_ok());
}

DMITIGR_JRPC_INLINE Error::Error(const std::error_code code,
  const int id, const std::string& message)
  : Error{code, message}
{
  init__(rapidjson::Value{id}, message);
  DMITIGR_ASSERT(is_invariant_ok());
}

DMITIGR_JRPC_INLINE Error::Error(const std::error_code code,
  const std::string_view id, const std::string& message)
  : Error{code, message}
{
  // Attention: calling allocator() assumes constructed rep_!
  init__(rapidjson::Value{id.data(), id.size(), allocator()}, message);
  DMITIGR_ASSERT(is_invariant_ok());
}

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

DMITIGR_JRPC_INLINE rapidjson::Value::AllocatorType& Error::allocator() const
{
  return rep_->GetAllocator();
}

// -----------------------------------------------------------------------------
// Private
// -----------------------------------------------------------------------------

DMITIGR_JRPC_INLINE bool Error::is_invariant_ok() const
{
  if (!rep_)
    return false;

  const auto mc = rep_->MemberCount();
  if (mc != 3)
    return false;

  const auto e = rep_->MemberEnd();

  const auto ji = rep_->FindMember("jsonrpc");
  if (ji == e || !(ji->value.IsString() && (rajson::to<std::string_view>(ji->value) == std::string_view{"2.0", 3})))
    return false;

  const auto ii = rep_->FindMember("id");
  if (ii == e || !(ii->value.IsInt() || ii->value.IsString() || ii->value.IsNull()))
    return false;

  const auto ei = rep_->FindMember("error");
  if (ei == e || !ei->value.IsObject())
    return false;

  const auto eimc = ei->value.MemberCount();
  const auto ee = ei->value.MemberEnd();
  const auto ci = ei->value.FindMember("code");
  if (ci == ee || !ci->value.IsInt())
    return false;

  const auto mi = ei->value.FindMember("message");
  if (mi == ee || !mi->value.IsString())
    return false;

  const auto di = ei->value.FindMember("data");
  if (di == ee) {
    if (eimc != 2)
      return false;
  } else if (eimc != 3)
    return false;

  return true;
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

DMITIGR_JRPC_INLINE Error::Error(const std::error_code code,
  rapidjson::Value&& id, const std::string& message)
  : Error{code, message}
{
  init__(std::move(id), message);
  DMITIGR_ASSERT(is_invariant_ok());
}

DMITIGR_JRPC_INLINE Error::Error(const std::error_code code,
  const rapidjson::Value& id, const std::string& message)
  : Error{code, message}
{
  init__(rapidjson::Value{id, allocator()}, message);
  DMITIGR_ASSERT(is_invariant_ok());
}

DMITIGR_JRPC_INLINE Error::Error(const std::error_code code,
  const std::string& message, std::shared_ptr<rapidjson::Document> rep)
  : system_error{code, message}
  , rep_{std::move(rep)}
{
  DMITIGR_ASSERT(rep_ != nullptr);
  // Maybe uninitialized, so invariant *maybe* invalid here!
}

DMITIGR_JRPC_INLINE Error::Error(const std::error_code code, const std::string& message)
  : Error{code, message, std::make_shared<rapidjson::Document>(rapidjson::kObjectType)}
{
  // Uninitialized, so invariant is invalid here!
}

DMITIGR_JRPC_INLINE void Error::init__(rapidjson::Value&& id, const std::string& message)
{
  auto& alloc = allocator();
  rep_->AddMember("jsonrpc", "2.0", alloc);
  rep_->AddMember("id", std::move(id), alloc);
  {
    using T = rapidjson::Type;
    using V = rapidjson::Value;
    V e{T::kObjectType};
    e.AddMember("code", V{code().value()}, alloc);
    e.AddMember("message", message, alloc);
    rep_->AddMember("error", std::move(e), alloc);
  }
}

} // namespace dmitigr jrpc
