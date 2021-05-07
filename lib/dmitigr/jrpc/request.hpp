// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#ifndef DMITIGR_JRPC_REQUEST_HPP
#define DMITIGR_JRPC_REQUEST_HPP

#include "response.hpp"
#include "../misc/math.hpp"
#include "../misc/str.hpp"

#include <algorithm>
#include <initializer_list>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace dmitigr::jrpc {

/// A request.
class Request final {
public:
  /// A parameter reference.
  class Paramref final {
  public:
    /// @returns `true` if the instance is valid (references a parameter).
    bool is_valid() const noexcept
    {
      return static_cast<bool>(value_);
    }

    /// @returns `is_valid()`.
    explicit operator bool() const noexcept
    {
      return is_valid();
    }

    /// @returns The request of this parameter.
    const Request& request() const noexcept
    {
      return request_;
    }

    /// @returns The pointer to the value of this parameter if `is_valid()`.
    const rapidjson::Value* value() const noexcept
    {
      return value_;
    }

    /// @returns The position or name of this parameter if `is_valid()`.
    const std::string& namepos() const noexcept
    {
      return namepos_;
    }

    /**
     * @returns The result of conversion of `value()` to a value of type `T`, or
     * `std::nullopt` if `(!value() || value()->IsNull())`.
     *
     * @throws Error if `value()` cannot be converted to a value of type `T`.
     */
    template<typename T>
    std::optional<T> optional() const
    {
      try {
        if (!value_ || value_->IsNull())
          return std::nullopt;
        else
          return rajson::to<T>(*value_);
      } catch (...) {
        throw_invalid_params();
      }
    }

    /**
     * @returns The result of conversion of `value()` to a value of type `T`, or
     * `std::nullopt` if `(!value() || value()->IsNull())`.
     *
     * @param pred An unary predicate with signature `bool(const T&)` that
     * returns `true` if a value of type `T` is valid.
     *
     * @throws Error if `value()` cannot be converted to a value of type `T`,
     * or if `!pred(T)`.
     */
    template<typename T, typename Predicate>
    std::enable_if_t<std::is_invocable_r_v<bool, Predicate, const T&>, std::optional<T>>
    optional(Predicate&& pred) const
    {
      if (auto result = optional<T>()) {
        if (pred(*result))
          return result;
        else
          throw_invalid_params();
      } else
        return std::nullopt;
    }

    /**
     * @overload
     *
     * @param valid_set A container of acceptable values.
     */
    template<typename T, typename U, typename A, template<class, class> class Container>
    std::optional<T> optional(const Container<U, A>& valid_set) const
    {
      return optional__<T>(valid_set);
    }

    /// @overload
    template<typename T, typename U>
    std::optional<T> optional(const std::initializer_list<U>& valid_set) const
    {
      return optional__<T>(valid_set);
    }

    /**
     * @overload
     *
     * @param interval An interval of acceptable values.
     */
    template<typename T, typename U>
    std::optional<T> optional(const math::Interval<U>& interval) const
    {
      return optional<T>([&interval](const auto& value){return interval.has(value);});
    }

    /**
     * @returns The result of conversion of `value()` to a value of type `T`.
     *
     * @params args The same arguments as for optional() methods.
     *
     * @throws Error if `!value() || value()->IsNull()`.
     */
    template<typename T, typename ... Types>
    T not_null(Types&& ... args) const
    {
      if (auto result = optional<T>(std::forward<Types>(args)...))
        return std::move(*result);
      else
        throw_invalid_params();
    }

    /// @overload
    template<typename T, typename U>
    T not_null(const std::initializer_list<U>& valid_set) const
    {
      if (auto result = optional<T>(valid_set))
        return std::move(*result);
      else
        throw_invalid_params();
    }

    [[noreturn]] void throw_invalid_params() const
    {
      request_.throw_error(Server_errc::invalid_params, namepos_);
    }

  private:
    friend Request;

    const Request& request_;
    const rapidjson::Value* value_{};
    std::string namepos_;

    template<typename T, typename U>
    std::optional<T> optional__(const U& valid_set) const
    {
      return optional<T>([&valid_set](const T& v)
      {
        return std::any_of(cbegin(valid_set), cend(valid_set), [&v](const auto& e)
        {
          return v == e;
        });
      });
    }

  public:
    explicit Paramref(const Request& request)
      : request_{request}
    {
      assert(!is_valid());
    }

    Paramref(const Request& request, const std::size_t pos, const rapidjson::Value& value)
      : request_{request}
      , value_{&value}
      , namepos_{std::to_string(pos)}
    {
      assert(is_valid());
    }

    Paramref(const Request& request, std::string name, const rapidjson::Value& value)
      : request_{request}
      , value_{&value}
      , namepos_{std::move(name)}
    {
      assert(is_valid());
    }

    /// Non copy-constructible.
    Paramref(const Paramref& rhs) = delete;

    /// Non copy-assignable.
    Paramref& operator=(const Paramref& rhs) = delete;

    /// Move-constructible.
    Paramref(Paramref&& rhs) = default;

    /// Move-assignable.
    Paramref& operator=(Paramref&& rhs) = default;
  };

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

  /// Constructs an instance that represents a normal request.
  Request(const Null /*id*/, std::string_view method)
  {
    init_request__(rapidjson::Value{}, method);
    assert(is_invariant_ok());
  }

  /// @overload
  Request(const int id, const std::string_view method)
  {
    init_request__(rapidjson::Value{id}, method);
    assert(is_invariant_ok());
  }

  /// @overload
  Request(const std::string_view id, const std::string_view method)
  {
    // Attention: calling allocator() assumes constructed rep_!
    init_request__(rapidjson::Value{id.data(), id.size(), allocator()}, method);
    assert(is_invariant_ok());
  }

  /// Constructs an instance that represents a notification.
  explicit Request(const std::string_view method)
  {
    init_notification__(method);
    assert(is_invariant_ok());
  }

  /// Copy-constructible.
  Request(const Request& rhs)
  {
    rep_.CopyFrom(rhs.rep_, allocator(), true);
  }

  /// Copy-assignable.
  Request& operator=(const Request& rhs)
  {
    if (this != &rhs) {
      Request tmp{rhs};
      swap(tmp);
    }
    return *this;
  }

  /// Move-constructible.
  Request(Request&& rhs) = default;

  /// Move-assignable.
  Request& operator=(Request&& rhs) = default;

  /// @}

  /// Swaps this instance with `rhs`.
  void swap(Request& rhs) noexcept
  {
    rep_.Swap(rhs.rep_);
  }

  /// @returns A String specifying the version of the JSON-RPC protocol.
  std::string_view jsonrpc() const
  {
    return rajson::to<std::string_view>(rep_.FindMember("jsonrpc")->value);
  }

  /**
   * @returns A request identifier which can be either a String, Number
   * or NULL, or `nullptr` if this instance represents a notification.
   */
  const rapidjson::Value* id() const noexcept
  {
    const auto i = rep_.FindMember("id");
    return i != rep_.MemberEnd() ? &i->value : nullptr;
  }

  /**
   * @returns A String containing the name of the method.
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
   * @returns A Structured value that holds the parameter values to be used
   * during the invocation of the method, or `nullptr` if no parameters.
   */
  const rapidjson::Value* params() const noexcept
  {
    const auto i = rep_.FindMember("params");
    return i != rep_.MemberEnd() ? &i->value : nullptr;
  }

  /**
   * @returns The parameter reference, or invalid instance if no parameter `position`.
   *
   * @par Requires
   * `(!params() || position < params()->Size())`.
   */
  Paramref parameter(const std::size_t position) const noexcept
  {
    if (const auto* const p = params(); p && p->IsArray()) {
      assert(position < p->Size());
      return {*this, position, (*p)[position]};
    } else
      return Paramref{*this};
  }

  /// @returns The parameter reference, or invalid instance if no parameter `name`.
  Paramref parameter(const std::string_view name) const noexcept
  {
    if (const auto* const p = params(); p && p->IsObject()) {
      const auto nr = rajson::to<rapidjson::Value::StringRefType>(name);
      const auto i = p->FindMember(nr);
      return i != p->MemberEnd() ? Paramref{*this, std::string{name}, i->value} : Paramref{*this};
    } else
      return Paramref{*this};
  }

  /**
   * @returns A value of type `std::tuple<Paramref, ...>`.
   *
   * @see parameters_mandatory(), parameters_not_null().
   */
  template<class ... Types>
  auto parameters(Types&& ... names) const
  {
    return std::make_tuple(parameter(std::forward<Types>(names))...);
  }

  /**
   * @returns A value of type `std::tuple<Paramref, ...>`.
   *
   * @throws `Server_errc::invalid_params` if any of parameters which are
   * specified in the `names` does not presents in request.
   *
   * @see parameters(), parameters_not_null().
   */
  template<class ... Types>
  auto parameters_mandatory(Types&& ... names) const
  {
    auto result = parameters(std::forward<Types>(names)...);
    if (!is_all_present(result))
      throw_error(Server_errc::invalid_params);
    return result;
  }

  /**
   * @returns A value of type `std::tuple<Paramref, ...>`.
   *
   * @throws `Server_errc::invalid_params` if any of parameters which are
   * specified in the `names` does not presents in request or `NULL`.
   *
   * @see parameters(), parameters_mandatory().
   */
  template<class ... Types>
  auto parameters_not_null(Types&& ... names) const
  {
    auto result = parameters(std::forward<Types>(names)...);
    if (!is_all_present(result))
      throw_error(Server_errc::invalid_params);
    return result;
  }

  /**
   * @brief Sets the method parameter of the specified `position` to the
   * specifid `value`.
   *
   * @par Requires
   * `(!params() || params()->IsArray())`.
   *
   * @par Effects
   * `(parameter(position) != nullptr)`.
   */
  void set_parameter(const std::size_t position, rapidjson::Value value)
  {
    auto& alloc = allocator();
    rapidjson::Value* p = params__();
    if (!p) {
      rep_.AddMember("params", rapidjson::Value{rapidjson::Type::kArrayType}, alloc);
      p = params__();
      p->Reserve(8, alloc);
    }
    assert(p && p->IsArray());

    if (position >= p->Size()) {
      const auto count = position - p->Size();
      for (std::size_t i = 0; i < count; ++i)
        p->PushBack(rapidjson::Value{}, alloc);
      p->PushBack(std::move(value), alloc);
      assert(position < p->Size());
    } else
      (*p)[position] = std::move(value);
  }

  /// @overload
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
   * `(!name.empty() && (!params() || params()->IsObject()))`.
   *
   * @par Effects
   * `(parameter(name) != nullptr)`.
   */
  void set_parameter(const std::string_view name, rapidjson::Value value)
  {
    assert(!name.empty());

    auto& alloc = allocator();
    rapidjson::Value* p = params__();
    if (!p) {
      rep_.AddMember("params", rapidjson::Value{rapidjson::Type::kObjectType}, alloc);
      p = params__();
    }
    assert(p && p->IsObject());

    const auto nr = rajson::to<rapidjson::Value::StringRefType>(name);
    if (auto m = p->FindMember(nr); m != p->MemberEnd())
      m->value = std::move(value);
    else
      p->AddMember(rapidjson::Value{std::string{name}, alloc}, std::move(value), alloc);
  }

  /// @overload
  template<typename T>
  void set_parameter(std::string_view name, T&& value)
  {
    set_parameter(name, rajson::to<rapidjson::Value>(std::forward<T>(value), allocator()));
  }

  /// @returns The parameter count. Returns 0 if `(params() == nullptr)`.
  std::size_t parameter_count() const noexcept
  {
    const auto* const p = params();
    return p ? (p->IsArray() ? p->Size() : p->MemberCount()) : 0;
  }

  /// @returns `(params() && parameter_count() > 0)`.
  bool has_parameters() const noexcept
  {
    return parameter_count() > 0;
  }

  /**
   * @brief Resets parameters and sets theirs notation.
   *
   * @par Effects
   * `(params() && parameter_count() == 0)`.
   */
  void reset_parameters(const Parameters_notation value)
  {
    if (auto* const p = params__()) {
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

    assert(params() && parameter_count() == 0);
  }

  /**
   * @brief Omits parameters.
   *
   * @par Effects
   * `(!params())`.
   */
  void omit_parameters()
  {
    rep_.RemoveMember("params");
  }

  /// @returns The result of serialization of this instance to a JSON string.
  std::string to_string() const
  {
    return rajson::to_stringified(rep_);
  }

  /// @return The allocator.
  rapidjson::Value::AllocatorType& allocator() const
  {
    return rep_.GetAllocator();
  }

  /**
   * @throws An instance of type Error with the specified `code`,
   * ID borrowed from this instance and specified error `message`.
   */
  [[noreturn]] void throw_error(const std::error_code code, const std::string& message = {}) const
  {
    if (id())
      throw Error{code, *id(), message};
    else
      throw Error{code, Null{}, message};
  }

  /**
   * @returns An instance of type Error with the specified `code`,
   * ID borrowed from this instance and specified error `message`.
   */
  Error make_error(const std::error_code code, const std::string& message = {}) const
  {
    if (id())
      return Error{code, *id(), message};
    else
      return Error{code, Null{}, message};
  }

  /// @returns An instance of type Result with ID borrowed from this instance.
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

  rapidjson::Value* params__() noexcept
  {
    return const_cast<rapidjson::Value*>(static_cast<const Request*>(this)->params());
  }

  template<std::size_t ... I, typename Tuple, typename Check>
  static bool is_all_passed__(std::index_sequence<I...>,
    const Tuple& tuple, Check&& check) noexcept
  {
    constexpr auto tsz = std::tuple_size<std::decay_t<decltype(tuple)>>();
    static_assert(sizeof...(I) == tsz);
    return (check(std::get<I>(tuple)) && ...);
  }

  template<typename Tuple>
  static bool is_all_present(const Tuple& tuple) noexcept
  {
    constexpr auto tsz = std::tuple_size<std::decay_t<decltype(tuple)>>();
    return is_all_passed__(std::make_index_sequence<tsz>{}, tuple,
      [](const auto& e){return e.value();});
  }

  template<typename Tuple>
  static bool is_all_not_null(const Tuple& tuple) noexcept
  {
    constexpr auto tsz = std::tuple_size<std::decay_t<decltype(tuple)>>();
    return is_all_passed__(std::make_index_sequence<tsz>{}, tuple,
      [](const auto& e){return e.value() && !e.value()->IsNull();});
  }

  // Used by from_json().
  Request(const std::string_view input, int)
    : rep_{rajson::to_document(input)}
  {
    if (rep_.HasParseError() || !rep_.IsObject())
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

    assert(is_invariant_ok());
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
