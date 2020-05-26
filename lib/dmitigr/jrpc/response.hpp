// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#ifndef DMITIGR_JRPC_RESPONSE_HPP
#define DMITIGR_JRPC_RESPONSE_HPP

#include "dmitigr/jrpc/basics.hpp"
#include "dmitigr/jrpc/std_system_error.hpp"
#include "dmitigr/jrpc/types_fwd.hpp"

#include <dmitigr/base/debug.hpp>
#include <dmitigr/rajson/conversions.hpp>

#include <memory>
#include <string>
#include <string_view>

namespace dmitigr::jrpc {

/**
 * @brief A response.
 */
class Response {
public:
  /**
   * @brief The destructor.
   */
  virtual ~Response() = default;

  /// @name Constructors
  /// @{

  /**
   * @returns A new instance of Response.
   */
  static std::unique_ptr<Response> make(std::string_view input);

  /// @}

  /**
   * @returns A String specifying the version of the JSON-RPC protocol.
   */
  virtual std::string_view jsonrpc() const = 0;

  /**
   * @returns A response identifier which the same as the value of the id member
   * in the Request. If there was an error in detecting the id in the Request
   * (e.g. Parse error/Invalid Request), the returned value is `Null`.
   */
  virtual const rapidjson::Value& id() const = 0;

  /**
   * @returns The result of serialization of this instance to a JSON string.
   */
  virtual std::string to_string() const = 0;

  /**
   * @return The allocator.
   */
  virtual rapidjson::Value::AllocatorType& allocator() const = 0;

private:
  friend Error;
  friend Result;

  Response() = default;
};

// =============================================================================

/**
 * @brief An error response.
 */
class Error final : public Response, public std::system_error {
public:
  /**
   * @brief Constructs an instance with code of Server_errc::generic_error,
   * with null ID, and empty message.
   */
  Error()
    : Error{Server_errc::generic_error, std::string{}}
  {
    init__(rapidjson::Value{}, std::string{});
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @brief The constructor.
   */
  Error(const std::error_code code, const Null /*id*/, const std::string& message = {})
    : Error{code, message}
  {
    init__(rapidjson::Value{}, message);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @overload
   */
  Error(const std::error_code code, const int id, const std::string& message = {})
    : Error{code, message}
  {
    init__(rapidjson::Value{id}, message);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @overload
   */
  Error(const std::error_code code,
    const std::string_view id, const std::string& message = {})
    : Error{code, message}
  {
    // Attention: calling allocator() assumes constructed rep_!
    init__(rapidjson::Value{id.data(), id.size(), allocator()}, message);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @brief Exchange the contents of this request with `other`.
   */
  void swap(Error& other)
  {
    rep_.swap(other.rep_);
  }

  /**
   * @see Response::jsonrpc().
   */
  std::string_view jsonrpc() const override
  {
    return std::string_view{"2.0", 3};
  }

  /**
   * @see Response::id().
   */
  const rapidjson::Value& id() const override
  {
    const auto i = rep_->FindMember("id");
    DMITIGR_ASSERT(i != rep_->MemberEnd());
    return i->value;
  }

  /**
   * @see Response::to_string().
   */
  std::string to_string() const override
  {
    return rajson::to_stringified(*rep_);
  }

  /**
   * @returns A Primitive or Structured value that contains additional
   * information about the error, or `nullptr` if omitted.
   */
  const rapidjson::Value* data() const
  {
    const auto& e = error();
    const auto i = e.FindMember("data");
    return i != e.MemberEnd() ? &i->value : nullptr;
  }

  /**
   * @brief Sets the additional information about the error.
   */
  void set_data(rapidjson::Value value)
  {
    auto& err = error();
    if (const auto i = err.FindMember("data"); i == err.MemberEnd())
      err.AddMember("data", std::move(value), allocator());
    else
      i->value = std::move(value);
  }

  /**
   * @overload
   */
  template<typename T>
  void set_data(T&& value)
  {
    set_data(rajson::to<rapidjson::Value>(std::forward<T>(value), allocator()));
  }

  /**
   * @see Response::allocator().
   */
  rapidjson::Value::AllocatorType& allocator() const override
  {
    return rep_->GetAllocator();
  }

private:
  friend Request;
  friend Response;

  std::shared_ptr<rapidjson::Document> rep_;

  bool is_invariant_ok() const
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

  const rapidjson::Value& error() const
  {
    const auto i = rep_->FindMember("error");
    DMITIGR_ASSERT(i != rep_->MemberEnd());
    return i->value;
  }

  rapidjson::Value& error()
  {
    return const_cast<rapidjson::Value&>(static_cast<const Error*>(this)->error());
  }

  // Used by Request
  Error(const std::error_code code, rapidjson::Value&& id, const std::string& message = {})
    : Error{code, message}
  {
    init__(std::move(id), message);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  // Used by Request
  Error(const std::error_code code, const rapidjson::Value& id, const std::string& message = {})
    : Error{code, message}
  {
    init__(rapidjson::Value{id, allocator()}, message);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  // Used by Response
  Error(const std::error_code code, const std::string& message, std::shared_ptr<rapidjson::Document> rep)
    : system_error{code, message}
    , rep_{std::move(rep)}
  {
    DMITIGR_ASSERT(rep_ != nullptr);
    // Maybe uninitialized, so invariant *maybe* invalid here!
  }

  // Used for pre initialization
  explicit Error(const std::error_code code, const std::string& message = {})
    : Error{code, message, std::make_shared<rapidjson::Document>(rapidjson::kObjectType)}
  {
    // Uninitialized, so invariant is invalid here!
  }

  void init__(rapidjson::Value&& id, const std::string& message)
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
};

// =============================================================================

/**
 * @brief Represents success of a server method invocation.
 */
class Result final : public Response {
public:
  /**
   * @brief The default constructor.
   */
  Result()
  {
    init__(rapidjson::Value{});
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns A new instance of result.
   */
  explicit Result(const int id)
  {
    init__(rapidjson::Value{id});
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @overload
   */
  explicit Result(const std::string_view id)
  {
    // Attention: calling allocator() assumes constructed rep_!
    init__(rapidjson::Value{id.data(), id.size(), allocator()});
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @brief Copy-constructable.
   */
  Result(const Result& rhs)
  {
    rep_.CopyFrom(rhs.rep_, allocator(), true);
  }

  /**
   * @brief Copy-assignable.
   */
  Result& operator=(const Result& rhs)
  {
    Result tmp{rhs};
    swap(tmp);
    return *this;
  }

  /**
   * @brief Move-constructable.
   */
  Result(Result&& rhs) = default;

  /**
   * @brief Move-assignable.
   */
  Result& operator=(Result&& rhs) = default;

  /**
   * @brief Exchange the contents of this request with `other`.
   */
  void swap(Result& other)
  {
    rep_.Swap(other.rep_);
  }

  /**
   * @see Response::jsonrpc()
   */
  std::string_view jsonrpc() const override
  {
    return std::string_view{"2.0", 3};
  }

  /**
   * @see Response::id()
   */
  const rapidjson::Value& id() const override
  {
    const auto i = rep_.FindMember("id");
    DMITIGR_ASSERT(i != rep_.MemberEnd());
    return i->value;
  }

  /**
   * @see Response::to_string()
   */
  std::string to_string() const override
  {
    return rajson::to_stringified(rep_);
  }

  /**
   * @see Response::allocator()
   */
  rapidjson::Value::AllocatorType& allocator() const override
  {
    return rep_.GetAllocator();
  }

  /**
   * @returns The value determined by the method invoked on the server.
   */
  const rapidjson::Value& data() const
  {
    const auto i = rep_.FindMember("result");
    DMITIGR_ASSERT(i != rep_.MemberEnd());
    return i->value;
  }

  /**
   * @brief Sets the mandatory information about the success.
   */
  void set_data(rapidjson::Value value)
  {
    data__() = std::move(value);
  }

  /**
   * @overload
   */
  template<typename T>
  void set_data(T&& value)
  {
    set_data(rajson::to<rapidjson::Value>(std::forward<T>(value), allocator()));
  }

private:
  friend Request;
  friend Response;

  mutable rapidjson::Document rep_{rapidjson::Type::kObjectType};

  bool is_invariant_ok() const
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

  rapidjson::Value& data__()
  {
    return const_cast<rapidjson::Value&>(static_cast<const Result*>(this)->data());
  }

  // Used by Request
  explicit Result(const rapidjson::Value& id)
  {
    init__(rapidjson::Value{id, allocator()});
    DMITIGR_ASSERT(is_invariant_ok());
  }

  // Used by Response
  explicit Result(rapidjson::Document&& rep)
    : rep_{std::move(rep)}
  {
    DMITIGR_ASSERT(is_invariant_ok());
  }

  void init__(rapidjson::Value&& id)
  {
    auto& alloc = allocator();
    rep_.AddMember("jsonrpc", "2.0", alloc);
    rep_.AddMember("result", rapidjson::Value{}, alloc);
    rep_.AddMember("id", std::move(id), alloc);
  }
};

// =============================================================================

inline std::unique_ptr<Response> Response::make(const std::string_view input)
{
  rapidjson::Document rep{rajson::to_document(input)};
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

#endif  // DMITIGR_JRPC_RESPONSE_HPP
