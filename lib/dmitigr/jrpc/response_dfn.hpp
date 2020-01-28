// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#ifndef DMITIGR_JRPC_RESPONSE_DFN_HPP
#define DMITIGR_JRPC_RESPONSE_DFN_HPP

#include "dmitigr/jrpc/dll.hpp"
#include "dmitigr/jrpc/types_fwd.hpp"
#include "dmitigr/rajson/conversions.hpp"

#include <memory>
#include <string_view>
#include <string>

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
  static DMITIGR_JRPC_API std::unique_ptr<Response> make(std::string_view input);

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
  virtual rapidjson::Value::AllocatorType& allocator() = 0;

private:
  friend Error;
  friend Result;

  Response() = default;
};

} // namespace dmitigr::jrpc

#endif  // DMITIGR_JRPC_RESPONSE_DFN_HPP
