// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#include "dmitigr/jrpc/result.hpp"
#include "dmitigr/jrpc/implementation_header.hpp"

#include "dmitigr/util/debug.hpp"

namespace dmitigr::jrpc::detail {

class iResult final : public Result {
public:
  explicit iResult(rapidjson::Value id)
  {
    auto& alloc = allocator();
    rep_.AddMember("jsonrpc", "2.0", alloc);
    rep_.AddMember("result", rapidjson::Value{}, alloc);
    rep_.AddMember("id", std::move(id), alloc);

    DMITIGR_ASSERT(is_invariant_ok());
  }

  iResult(const std::optional<int> id)
    : iResult{id ? rapidjson::Value{*id} : rapidjson::Value{}}
  {}

  iResult(const std::string_view id)
    : iResult{rapidjson::Value{id.data(), id.size(), allocator()}}
  {}

  iResult(rapidjson::Document rep)
    : rep_{std::move(rep)}
  {
    DMITIGR_ASSERT(is_invariant_ok());
  }

  // ---------------------------------------------------------------------------
  // Response overridings
  // ---------------------------------------------------------------------------

  std::string_view jsonrpc() const override
  {
    return std::string_view{"2.0", 3};
  }

  const rapidjson::Value& id() const override
  {
    const auto i = rep_.FindMember("id");
    DMITIGR_ASSERT(i != rep_.MemberEnd());
    return i->value;
  }

  std::string to_string() const override
  {
    return rajson::to_stringified_json(rep_);
  }

  // ---------------------------------------------------------------------------
  // Result overridings
  // ---------------------------------------------------------------------------

  const rapidjson::Value& data() const override
  {
    const auto i = rep_.FindMember("result");
    DMITIGR_ASSERT(i != rep_.MemberEnd());
    return i->value;
  }

  void set_data(rapidjson::Value value) override
  {
    data() = std::move(value);
  }

  rapidjson::Value::AllocatorType& allocator() override
  {
    return rep_.GetAllocator();
  }

private:
  rapidjson::Document rep_{rapidjson::Type::kObjectType};

  rapidjson::Value& data()
  {
    return const_cast<rapidjson::Value&>(static_cast<const iResult*>(this)->data());
  }

  bool is_invariant_ok()
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
};

} // namespace dmitigr::jrpc::detail

namespace dmitigr::jrpc {

DMITIGR_JRPC_INLINE std::unique_ptr<Result> Result::make(const std::optional<int> id)
{
  return std::make_unique<detail::iResult>(id);
}

DMITIGR_JRPC_INLINE std::unique_ptr<Result> Result::make(const std::string_view id)
{
  return std::make_unique<detail::iResult>(id);
}

DMITIGR_JRPC_INLINE std::unique_ptr<Result> Result::make(rapidjson::Document rep)
{
  return std::make_unique<detail::iResult>(std::move(rep));
}

} // namespace dmitigr::jrpc

#include "dmitigr/jrpc/implementation_footer.hpp"
