// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#ifndef DMITIGR_JRPC_REQUEST_HPP
#define DMITIGR_JRPC_REQUEST_HPP

#include "dmitigr/jrpc/basics.hpp"
#include "dmitigr/jrpc/dll.hpp"
#include "dmitigr/jrpc/types_fwd.hpp"
#include "dmitigr/rajson/conversions.hpp"

#include <string>
#include <string_view>

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
  static DMITIGR_JRPC_API Request from_json(std::string_view input);

  /**
   * @brief Constructs an instance that represents a normal request.
   */
  DMITIGR_JRPC_API Request(Null id, std::string_view method);

  /**
   * @overload
   */
  DMITIGR_JRPC_API Request(int id, std::string_view method);

  /**
   * @overload
   */
  DMITIGR_JRPC_API Request(std::string_view id, std::string_view method);

  /**
   * @brief Constructs an instance that represents a notification.
   */
  DMITIGR_JRPC_API Request(std::string_view method);

  /**
   * @brief Non copy-constructable.
   */
  Request(const Request&) = delete;

  /**
   * @brief Non copy-assignable.
   */
  Request& operator=(const Request&) = delete;

  /**
   * @brief Move-constructable.
   */
  Request(Request&&) = default;

  /**
   * @brief Move-assignable.
   */
  Request& operator=(Request&&) = default;

  /// @}

  /**
   * @returns A String specifying the version of the JSON-RPC protocol.
   */
  DMITIGR_JRPC_API std::string_view jsonrpc() const;

  /**
   * @returns A request identifier which can be either a String, Number or NULL,
   * or `nullptr` if this instance represents a notification.
   */
  DMITIGR_JRPC_API const rapidjson::Value* id() const;

  /**
   * @returns A String containing the name of the method to be invoked.
   *
   * @remarks Method names that begin with the word "rpc" followed by a period
   * character (U+002E or ASCII 46) are reserved for rpc-internal methods and
   * extensions.
   */
  DMITIGR_JRPC_API std::string_view method() const;

  /**
   * @returns A Structured value that holds the parameter values to be
   * used during the invocation of the method, or `nullptr` if no parameters.
   */
  DMITIGR_JRPC_API const rapidjson::Value* parameters() const;

  /**
   * @returns The parameter value, or `nullptr` if no parameter at `position`.
   */
  DMITIGR_JRPC_API const rapidjson::Value* parameter(std::size_t position) const;

  /**
   * @returns The parameter value, or `nullptr` if no parameter with name `name`.
   */
  DMITIGR_JRPC_API const rapidjson::Value* parameter(std::string_view name) const;

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
  DMITIGR_JRPC_API void set_parameter(std::size_t position, rapidjson::Value value);

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
  DMITIGR_JRPC_API void set_parameter(std::string_view name, rapidjson::Value value);

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
  DMITIGR_JRPC_API std::size_t parameter_count() const;

  /**
   * @returns `(parameters() && parameter_count() > 0)`.
   */
  DMITIGR_JRPC_API bool has_parameters() const;

  /**
   * @brief Resets parameters and sets theirs notation.
   *
   * @par Effects
   * `(parameters() && parameter_count() == 0)`.
   */
  DMITIGR_JRPC_API void reset_parameters(Parameters_notation value);

  /**
   * @brief Omits parameters.
   *
   * @par Effects
   * `(!parameters())`.
   */
  DMITIGR_JRPC_API void omit_parameters();

  /**
   * @returns The result of serialization of this instance to a JSON string.
   */
  DMITIGR_JRPC_API std::string to_string() const;

  /**
   * @return The allocator.
   */
  DMITIGR_JRPC_API rapidjson::Value::AllocatorType& allocator();

private:
  rapidjson::Document rep_{rapidjson::Type::kObjectType};

  Request(const std::string_view input, int);
  Request(rapidjson::Value id, const std::string_view method);

  bool is_invariant_ok() const;
  void init__(const std::string_view method);
  rapidjson::Value* parameters__();
};

} // namespace dmitigr::jrpc

#ifdef DMITIGR_JRPC_HEADER_ONLY
#include "dmitigr/jrpc/request.cpp"
#endif

#endif  // DMITIGR_JRPC_REQUEST_HPP
