// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#include "dmitigr/jrpc/basics.hpp"
#include "dmitigr/jrpc/error.hpp"
#include "dmitigr/str/str.hpp"
#include "dmitigr/util/debug.hpp"

namespace dmitigr::jrpc::detail {

/**
 * @brief Implementation of Request.
 */
class iRequest final : public Request {
public:
  explicit iRequest(const std::string_view input)
    : rep_{rajson::to_parsed_json(input)}
  {
    if (rep_.HasParseError())
      throw Error{Server_errc::parse_error, rapidjson::Value{}};

    std::size_t expected_member_count = 4;
    const auto e = rep_.MemberEnd();

    // Checking id member. (Absence of id indicates notification.)
    const auto idi = rep_.FindMember("id");
    if (idi != e) {
      if (!idi->value.IsNumber() && !idi->value.IsString() && !idi->value.IsNull())
        throw Error{Server_errc::invalid_request, rapidjson::Value{}, "invalid type of \"id\" member"};
    } else
      expected_member_count--;

    const auto throw_invalid_request = [this, idi, e](const std::string& message)
    {
      throw Error{Server_errc::invalid_request,
          (idi != e) ? rapidjson::Value{idi->value, allocator()} : rapidjson::Value{}, message};
    };

    // Checking jsonrpc member.
    if (const auto i = rep_.FindMember("jsonrpc"); i != e) {
      if (i->value.IsString()) {
        const auto jsonrpc = rajson::to<std::string_view>(i->value);
        if (jsonrpc != std::string_view{"2.0", 3})
          throw_invalid_request("invalid value of \"jsonrpc\" member");
      } else
        throw_invalid_request("invalid type of \"jsonrpc\" member");
    } else
      throw_invalid_request("no \"jsonrpc\" member found");

    // Checking method member.
    if (const auto i = rep_.FindMember("method"); i != e) {
      if (i->value.IsString()) {
        const auto method = rajson::to<std::string_view>(i->value);
        if (str::is_begins_with(method, std::string_view{"rpc.", 4}))
          throw_invalid_request("method names that begin with \"rpc.\" are reserved");
      } else
        throw_invalid_request("invalid type of \"method\" member");
    } else
      throw_invalid_request("no \"method\" member found");

    // Checking params member
    if (const auto i = rep_.FindMember("params"); i != e) {
      if (!i->value.IsArray() && !i->value.IsObject())
        throw_invalid_request("invalid type of \"params\" member");
    } else
      expected_member_count--;

    if (rep_.MemberCount() != expected_member_count)
      throw_invalid_request("unexpected member count");

    DMITIGR_ASSERT(is_invariant_ok());
  }

  // Constructs notification.
  iRequest(const std::string_view method, int)
  {
    init__(method);

    DMITIGR_ASSERT(is_invariant_ok());
  }

  iRequest(rapidjson::Value id, const std::string_view method)
  {
    init__(method);
    rep_.AddMember("id", std::move(id), allocator());

    DMITIGR_ASSERT(is_invariant_ok());
  }

  iRequest(const std::optional<int> id, const std::string_view method)
    : iRequest{id ? rapidjson::Value{*id} : rapidjson::Value{}, method}
  {}

  iRequest(const std::string_view id, const std::string_view method)
    : iRequest{rapidjson::Value{id.data(), id.size(), allocator()}, method}
  {}

  std::string_view jsonrpc() const override
  {
    return rajson::to<std::string_view>(rep_.FindMember("jsonrpc")->value);
  }

  const rapidjson::Value* id() const override
  {
    const auto i = rep_.FindMember("id");
    return i != rep_.MemberEnd() ? &i->value : nullptr;
  }

  std::string_view method() const override
  {
    return rajson::to<std::string_view>(rep_.FindMember("method")->value);
  }

  const rapidjson::Value* parameters() const override
  {
    const auto i = rep_.FindMember("params");
    return i != rep_.MemberEnd() ? &i->value : nullptr;
  }

  const rapidjson::Value* parameter(const std::size_t position) const override
  {
    if (const auto* const p = parameters(); p && p->IsArray()) {
      DMITIGR_REQUIRE(position < p->Size(), std::invalid_argument);
      return &(*p)[position];
    } else
      return nullptr;
  }

  const rapidjson::Value* parameter(const std::string_view name) const override
  {
    if (const auto* const p = parameters(); p && p->IsObject()) {
      const auto nr = rajson::to<rapidjson::Value::StringRefType>(name);
      const auto i = p->FindMember(nr);
      return i != p->MemberEnd() ? &i->value : nullptr;
    } else
      return nullptr;
  }

  void set_parameter(const std::size_t position, rapidjson::Value value) override
  {
    auto& alloc = allocator();
    rapidjson::Value* p = parameters();
    if (!p) {
      rep_.AddMember("params", rapidjson::Value{rapidjson::Type::kArrayType}, alloc);
      p = parameters();
      p->Reserve(8, alloc);
      DMITIGR_ASSERT(p && p->IsArray());
    } else
      DMITIGR_REQUIRE(p->IsArray(), std::logic_error);

    if (position >= p->Size()) {
      const auto count = position - p->Size();
      for (std::size_t i = 0; i < count; ++i)
        p->PushBack(rapidjson::Value{}, alloc);
      p->PushBack(std::move(value), alloc);
      DMITIGR_ASSERT(position < p->Size());
    } else
      (*p)[position] = std::move(value);
  }

  void set_parameter(const std::string_view name, rapidjson::Value value) override
  {
    DMITIGR_REQUIRE(!name.empty(), std::invalid_argument);

    auto& alloc = allocator();
    rapidjson::Value* p = parameters();
    if (!p) {
      rep_.AddMember("params", rapidjson::Value{rapidjson::Type::kObjectType}, alloc);
      p = parameters();
      DMITIGR_ASSERT(p && p->IsObject());
    } else
      DMITIGR_REQUIRE(p->IsObject(), std::logic_error);

    const auto nr = rajson::to<rapidjson::Value::StringRefType>(name);
    if (auto m = p->FindMember(nr); m != p->MemberEnd())
      m->value = std::move(value);
    else
      p->AddMember(rapidjson::Value{std::string{name}, alloc}, std::move(value), alloc);
  }

  std::size_t parameter_count() const override
  {
    const auto* const p = parameters();
    return p ? (p->IsArray() ? p->Size() : p->MemberCount()) : 0;
  }

  bool has_parameters() const override
  {
    return parameter_count() > 0;
  }

  void reset_parameters(const Parameters_notation value) override
  {
    if (auto* const p = parameters()) {
      if (value == Parameters_notation::positional) {
        if (p->IsArray())
          p->Clear();
        else
          p->SetArray();
      } else {
        if (p->IsObject())
          p->RemoveAllMembers();
        else
          p->SetObject();
      }
    } else if (value == Parameters_notation::positional)
      rep_.AddMember("params", rapidjson::Value{rapidjson::Type::kArrayType}, allocator());
    else
      rep_.AddMember("params", rapidjson::Value{rapidjson::Type::kObjectType}, allocator());

    DMITIGR_ASSERT(parameters() && parameter_count() == 0);
  }

  void omit_parameters() override
  {
    rep_.RemoveMember("params");
  }

  std::string to_string() const override
  {
    return rajson::to_stringified_json(rep_);
  }

  rapidjson::Value::AllocatorType& allocator() override
  {
    return rep_.GetAllocator();
  }

private:
  void init__(const std::string_view method)
  {
    auto& alloc = allocator();
    rep_.AddMember("jsonrpc", "2.0", alloc);
    rep_.AddMember("method", rapidjson::Value{method.data(), method.size(), alloc}, alloc);
  }

  bool is_invariant_ok() const
  {
    const auto e = rep_.MemberEnd();
    const auto ji = rep_.FindMember("jsonrpc");
    const auto mi = rep_.FindMember("method");
    const auto pi = rep_.FindMember("params");
    const auto ii = rep_.FindMember("id");

    return ji != e && mi != e &&
      (rajson::to<std::string_view>(ji->value) == std::string_view{"2.0", 3}) &&
      (pi == e || pi->value.IsObject() || pi->value.IsArray()) &&
      (ii == e || ii->value.IsInt() || ii->value.IsString() || ii->value.IsNull());
  }

  rapidjson::Value* parameters()
  {
    return const_cast<rapidjson::Value*>(static_cast<const iRequest*>(this)->parameters());
  }

  rapidjson::Document rep_{rapidjson::Type::kObjectType};
};

} // namespace dmitigr::jrpc::detail

namespace dmitigr::jrpc {

DMITIGR_JRPC_INLINE std::unique_ptr<Request> Request::make(const std::string_view input)
{
  return std::make_unique<detail::iRequest>(input);
}

DMITIGR_JRPC_INLINE std::unique_ptr<Request> Request::make(const std::optional<int> id, const std::string_view method)
{
  return std::make_unique<detail::iRequest>(id, method);
}

DMITIGR_JRPC_INLINE std::unique_ptr<Request> Request::make(const std::string_view id, const std::string_view method)
{
  return std::make_unique<detail::iRequest>(id, method);
}

DMITIGR_JRPC_INLINE std::unique_ptr<Request> Request::make_notification(const std::string_view method)
{
  return std::make_unique<detail::iRequest>(method, int{});
}

} // namespace dmitigr::jrpc
