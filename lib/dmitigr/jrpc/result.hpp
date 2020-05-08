// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#ifndef DMITIGR_JRPC_RESULT_HPP
#define DMITIGR_JRPC_RESULT_HPP

#include "dmitigr/jrpc/response_dfn.hpp"
#include "dmitigr/rajson/conversions.hpp"

namespace dmitigr::jrpc {

/**
 * @brief Represents success of a server method invocation.
 */
class Result final : public Response {
public:
  /**
   * @brief The default constructor.
   */
  DMITIGR_JRPC_API Result();

  /**
   * @returns A new instance of result.
   */
  DMITIGR_JRPC_API Result(int id);

  /**
   * @overload
   */
  DMITIGR_JRPC_API Result(std::string_view id);

  /**
   * @brief Copy-constructable.
   */
  DMITIGR_JRPC_API Result(const Result& rhs);

  /**
   * @brief Copy-assignable.
   */
  DMITIGR_JRPC_API Result& operator=(const Result& rhs);

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
  DMITIGR_JRPC_API void swap(Result& other);

  /**
   * @see Response::jsonrpc()
   */
  DMITIGR_JRPC_API std::string_view jsonrpc() const override;

  /**
   * @see Response::id()
   */
  DMITIGR_JRPC_API const rapidjson::Value& id() const override;

  /**
   * @see Response::to_string()
   */
  DMITIGR_JRPC_API std::string to_string() const override;

  /**
   * @see Response::allocator()
   */
  DMITIGR_JRPC_API rapidjson::Value::AllocatorType& allocator() const override;

  /**
   * @returns The value determined by the method invoked on the server.
   */
  DMITIGR_JRPC_API const rapidjson::Value& data() const;

  /**
   * @brief Sets the mandatory information about the success.
   */
  DMITIGR_JRPC_API void set_data(rapidjson::Value value);

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

  bool is_invariant_ok() const;
  rapidjson::Value& data__();

  explicit Result(const rapidjson::Value& id); // used by Request
  explicit Result(rapidjson::Document&& rep); // used by Response

  void init__(rapidjson::Value&& id);
};

} // namespace dmitigr::jrpc

#ifdef DMITIGR_JRPC_HEADER_ONLY
#include "dmitigr/jrpc/result.cpp"
#endif

#endif  // DMITIGR_JRPC_RESULT_HPP
