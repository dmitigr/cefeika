// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#ifndef DMITIGR_JRPC_RESULT_HPP
#define DMITIGR_JRPC_RESULT_HPP

#include "dmitigr/jrpc/response_dfn.hpp"
#include "dmitigr/rajson/conversions.hpp"

#include <optional>

namespace dmitigr::jrpc {

/**
 * @brief Represents success of a server method invocation.
 */
class Result : public Response {
public:
  /**
   * @returns A new instance of result.
   */
  static DMITIGR_JRPC_API std::unique_ptr<Result> make(std::optional<int> id);

  /**
   * @overload
   */
  static DMITIGR_JRPC_API std::unique_ptr<Result> make(std::string_view id);

  /**
   * @returns The value determined by the method invoked on the server.
   */
  virtual const rapidjson::Value& data() const = 0;

  /**
   * @brief Sets the additional information about the error.
   */
  virtual void set_data(rapidjson::Value value) = 0;

  /**
   * @overload
   */
  template<typename T>
  void set_data(T&& value)
  {
    set_data(rajson::to<rapidjson::Value>(std::forward<T>(value), allocator()));
  }

private:
  friend detail::iResult;
  friend Response;

  Result() = default;

  static std::unique_ptr<Result> make(rapidjson::Document rep);
};

} // namespace dmitigr::jrpc

#ifdef DMITIGR_JRPC_HEADER_ONLY
#include "dmitigr/jrpc/result.cpp"
#endif

#endif  // DMITIGR_JRPC_RESULT_HPP
