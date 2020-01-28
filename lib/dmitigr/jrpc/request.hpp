// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#ifndef DMITIGR_JRPC_REQUEST_HPP
#define DMITIGR_JRPC_REQUEST_HPP

#include "dmitigr/jrpc/dll.hpp"
#include "dmitigr/jrpc/types_fwd.hpp"
#include "dmitigr/rajson/conversions.hpp"

#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace dmitigr::jrpc {

/**
 * @brief A request.
 */
class Request {
public:
  /**
   * @brief The destructor.
   */
  virtual ~Request() = default;

  /// @name Constructors
  /// @{

  /**
   * @returns A new instance of Request.
   */
  static DMITIGR_JRPC_API std::unique_ptr<Request> make(std::string_view input);

  /**
   * @overload
   */
  static DMITIGR_JRPC_API std::unique_ptr<Request> make(std::optional<int> id, std::string_view method);

  /**
   * @overload
   */
  static DMITIGR_JRPC_API std::unique_ptr<Request> make(std::string_view id, std::string_view method);

  /**
   * @returns A new instance of Request that represents notification.
   */
  static DMITIGR_JRPC_API std::unique_ptr<Request> make_notification(std::string_view method);

  /// @}

  /**
   * @returns A String specifying the version of the JSON-RPC protocol.
   */
  virtual std::string_view jsonrpc() const = 0;

  /**
   * @returns A request identifier which can be either a String, Number or NULL,
   * or `nullptr` if this instance represents a notification.
   */
  virtual const rapidjson::Value* id() const = 0;

  /**
   * @returns A String containing the name of the method to be invoked.
   *
   * @remarks Method names that begin with the word "rpc" followed by a period
   * character (U+002E or ASCII 46) are reserved for rpc-internal methods and
   * extensions.
   */
  virtual std::string_view method() const = 0;

  /**
   * @returns A Structured value that holds the parameter values to be
   * used during the invocation of the method, or `nullptr` if no parameters.
   */
  virtual const rapidjson::Value* parameters() const = 0;

  /**
   * @returns The parameter value, or `nullptr` if no parameter at `position`.
   */
  virtual const rapidjson::Value* parameter(std::size_t position) const = 0;

  /**
   * @returns The parameter value, or `nullptr` if no parameter with name `name`.
   */
  virtual const rapidjson::Value* parameter(std::string_view name) const = 0;

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
  virtual void set_parameter(std::size_t position, rapidjson::Value value) = 0;

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
  virtual void set_parameter(std::string_view name, rapidjson::Value value) = 0;

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
  virtual std::size_t parameter_count() const = 0;

  /**
   * @returns `(parameters() && parameter_count() > 0)`.
   */
  virtual bool has_parameters() const = 0;

  /**
   * @brief Resets parameters and sets theirs notation.
   *
   * @par Effects
   * `(parameters() && parameter_count() == 0)`.
   */
  virtual void reset_parameters(Parameters_notation value) = 0;

  /**
   * @brief Omits parameters.
   *
   * @par Effects
   * `(!parameters())`.
   */
  virtual void omit_parameters() = 0;

  /**
   * @returns The result of serialization of this instance to a JSON string.
   */
  virtual std::string to_string() const = 0;

  /**
   * @return The allocator.
   */
  virtual rapidjson::Value::AllocatorType& allocator() = 0;

private:
  friend detail::iRequest;

  Request() = default;
};

} // namespace dmitigr::jrpc

#ifdef DMITIGR_JRPC_HEADER_ONLY
#include "dmitigr/jrpc/request.cpp"
#endif

#endif  // DMITIGR_JRPC_REQUEST_HPP
