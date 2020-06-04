// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#ifndef DMITIGR_JRPC_REQUEST_HPP
#define DMITIGR_JRPC_REQUEST_HPP

#include "dmitigr/jrpc/response.hpp"
#include <dmitigr/str/str.hpp>

namespace dmitigr::jrpc {

/**
 * @brief A request.
 */
class Request final {
public:
  /// @name Constructors
  /// @{

  /**
   * @returns A new instance that represents either normal request or notification.
   *
   * @param input Stringified JSON that represents a JSON-RPC request.
   */
  static Request from_json(const std::string_view input)
  {
    return Request{input, int{}};
  }

  /**
   * @brief Constructs an instance that represents a normal request.
   */
  Request(const Null /*id*/, std::string_view method)
  {
    init_request__(rapidjson::Value{}, method);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @overload
   */
  Request(const int id, const std::string_view method)
  {
    init_request__(rapidjson::Value{id}, method);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @overload
   */
  Request(const std::string_view id, const std::string_view method)
  {
    // Attention: calling allocator() assumes constructed rep_!
    init_request__(rapidjson::Value{id.data(), id.size(), allocator()}, method);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @brief Constructs an instance that represents a notification.
   */
  explicit Request(const std::string_view method)
  {
    init_notification__(method);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @brief Copy-constructable.
   */
  Request(const Request& rhs)
  {
    rep_.CopyFrom(rhs.rep_, allocator(), true);
  }

  /**
   * @brief Copy-assignable.
   */
  Request& operator=(const Request& rhs)
  {
    Request tmp{rhs};
    swap(tmp);
    return *this;
  }

  /**
   * @brief Move-constructable.
   */
  Request(Request&& rhs) = default;

  /**
   * @brief Move-assignable.
   */
  Request& operator=(Request&& rhs) = default;

  /// @}

  /**
   * @brief Exchange the contents of this request with `other`.
   */
  void swap(Request& other)
  {
    rep_.Swap(other.rep_);
  }

  /**
   * @returns A String specifying the version of the JSON-RPC protocol.
   */
  std::string_view jsonrpc() const
  {
    return rajson::to<std::string_view>(rep_.FindMember("jsonrpc")->value);
  }

  /**
   * @returns A request identifier which can be either a String, Number or NULL,
   * or `nullptr` if this instance represents a notification.
   */
  const rapidjson::Value* id() const
  {
    const auto i = rep_.FindMember("id");
    return i != rep_.MemberEnd() ? &i->value : nullptr;
  }

  /**
   * @returns A String containing the name of the method to be invoked.
   *
   * @remarks Method names that begin with the word "rpc" followed by a period
   * character (U+002E or ASCII 46) are reserved for rpc-internal methods and
   * extensions.
   */
  std::string_view method() const
  {
    return rajson::to<std::string_view>(rep_.FindMember("method")->value);
  }

  /**
   * @returns A Structured value that holds the parameter values to be
   * used during the invocation of the method, or `nullptr` if no parameters.
   */
  const rapidjson::Value* parameters() const
  {
    const auto i = rep_.FindMember("params");
    return i != rep_.MemberEnd() ? &i->value : nullptr;
  }

  /**
   * @returns The parameter value, or `nullptr` if no parameter at `position`.
   */
  const rapidjson::Value* parameter(const std::size_t position) const
  {
    if (const auto* const p = parameters(); p && p->IsArray()) {
      DMITIGR_REQUIRE(position < p->Size(), std::invalid_argument);
      return &(*p)[position];
    } else
      return nullptr;
  }

  /**
   * @returns The parameter value, or `nullptr` if no parameter with name `name`.
   */
  const rapidjson::Value* parameter(const std::string_view name) const
  {
    if (const auto* const p = parameters(); p && p->IsObject()) {
      const auto nr = rajson::to<rapidjson::Value::StringRefType>(name);
      const auto i = p->FindMember(nr);
      return i != p->MemberEnd() ? &i->value : nullptr;
    } else
      return nullptr;
  }

  /**
   * @brief Sets the method parameter of the specified `position` to the
   * specifid `value`.
   *
   * @par Requires
   * `(!parameters() || parameters()->IsArray())`.
   *
   * @par Effects
   * `(parameter(position) != nullptr)`.
   */
  void set_parameter(const std::size_t position, rapidjson::Value value)
  {
    auto& alloc = allocator();
    rapidjson::Value* p = parameters__();
    if (!p) {
      rep_.AddMember("params", rapidjson::Value{rapidjson::Type::kArrayType}, alloc);
      p = parameters__();
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

  /**
   * @overload
   */
  template<typename T>
  void set_parameter(std::size_t position, T&& value)
  {
    set_parameter(position, rajson::to<rapidjson::Value>(std::forward<T>(value), allocator()));
  }

  /**
   * @brief Sets the method parameter of the specified `name` to the
   * specified `value`.
   *
   * @par Requires
   * `(!name.empty() && (!parameters() || parameters()->IsObject()))`.
   *
   * @par Effects
   * `(parameter(name) != nullptr)`.
   */
  void set_parameter(const std::string_view name, rapidjson::Value value)
  {
    DMITIGR_REQUIRE(!name.empty(), std::invalid_argument);

    auto& alloc = allocator();
    rapidjson::Value* p = parameters__();
    if (!p) {
      rep_.AddMember("params", rapidjson::Value{rapidjson::Type::kObjectType}, alloc);
      p = parameters__();
      DMITIGR_ASSERT(p && p->IsObject());
    } else
      DMITIGR_REQUIRE(p->IsObject(), std::logic_error);

    const auto nr = rajson::to<rapidjson::Value::StringRefType>(name);
    if (auto m = p->FindMember(nr); m != p->MemberEnd())
      m->value = std::move(value);
    else
      p->AddMember(rapidjson::Value{std::string{name}, alloc}, std::move(value), alloc);
  }

  /**
   * @overload
   */
  template<typename T>
  void set_parameter(std::string_view name, T&& value)
  {
    set_parameter(name, rajson::to<rapidjson::Value>(std::forward<T>(value), allocator()));
  }

  /**
   * @returns The parameter count. Returns 0 if `(parameters() == nullptr)`.
   */
  std::size_t parameter_count() const
  {
    const auto* const p = parameters();
    return p ? (p->IsArray() ? p->Size() : p->MemberCount()) : 0;
  }

  /**
   * @returns `(parameters() && parameter_count() > 0)`.
   */
  bool has_parameters() const
  {
    return parameter_count() > 0;
  }

  /**
   * @brief Resets parameters and sets theirs notation.
   *
   * @par Effects
   * `(parameters() && parameter_count() == 0)`.
   */
  void reset_parameters(const Parameters_notation value)
  {
    if (auto* const p = parameters__()) {
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

  /**
   * @brief Omits parameters.
   *
   * @par Effects
   * `(!parameters())`.
   */
  void omit_parameters()
  {
    rep_.RemoveMember("params");
  }

  /**
   * @returns The result of serialization of this instance to a JSON string.
   */
  std::string to_string() const
  {
    return rajson::to_stringified(rep_);
  }

  /**
   * @return The allocator.
   */
  rapidjson::Value::AllocatorType& allocator() const
  {
    return rep_.GetAllocator();
  }

  /**
   * @brief Throws an instance of type Error with specified `code`, ID borrowed
   * from this instance and specified error `message`.
   */
  [[noreturn]] void throw_error(const std::error_code code, const std::string& message = {}) const
  {
    DMITIGR_REQUIRE(id(), std::logic_error, "throwing errors for notifications is nonsense");
    throw Error{code, *id(), message};
  }

  /**
   * @returns An instance of type Error with specified `code`, ID borrowed
   * from this instance and specified error `message`.
   */
  Error make_error(const std::error_code code, const std::string& message = {}) const
  {
    DMITIGR_REQUIRE(id(), std::logic_error, "making errors for notifications is nonsense");
    return Error{code, *id(), message};
  }

  /**
   * @returns An instance of type Result with ID borrowed
   * from this instance.
   */
  Result make_result() const
  {
    if (const auto* const ident = id())
      return Result{*ident};
    else
      return Result{};
  }

  /**
   * @overload
   *
   * @param value A value to set as the result data.
   */
  template<typename T>
  Result make_result(T&& value) const
  {
    auto result = make_result();
    result.set_data(std::forward<T>(value));
    return result;
  }

private:
  mutable rapidjson::Document rep_{rapidjson::Type::kObjectType};

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

  rapidjson::Value* parameters__()
  {
    return const_cast<rapidjson::Value*>(static_cast<const Request*>(this)->parameters());
  }

  // for from_json
  Request(const std::string_view input, int)
    : rep_{rajson::to_document(input)}
  {
    if (rep_.HasParseError())
      throw Error{Server_errc::parse_error, null};

    std::size_t expected_member_count = 4;
    const auto e = rep_.MemberEnd();

    // Checking id member. (Absence of id indicates notification.)
    const auto idi = rep_.FindMember("id");
    if (idi != e) {
      if (!idi->value.IsNumber() && !idi->value.IsString() && !idi->value.IsNull())
        throw Error{Server_errc::invalid_request, null, "invalid type of \"id\" member"};
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

  void init_notification__(const std::string_view method)
  {
    auto& alloc = allocator();
    rep_.AddMember("jsonrpc", "2.0", alloc);
    rep_.AddMember("method", rapidjson::Value{method.data(), method.size(), alloc}, alloc);
  }

  void init_request__(rapidjson::Value&& id, const std::string_view method)
  {
    init_notification__(method);
    rep_.AddMember("id", std::move(id), allocator());
  }
};

} // namespace dmitigr::jrpc

#endif  // DMITIGR_JRPC_REQUEST_HPP
