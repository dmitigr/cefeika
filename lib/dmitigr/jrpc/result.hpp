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
   * @brief Non copy-constructable.
   */
  Result(const Result&) = delete;

  /**
   * @brief Non copy-assignable.
   */
  Result& operator=(const Result&) = delete;

  /**
   * @brief Move-constructable.
   */
  Result(Result&&) = default;

  /**
   * @brief Move-assignable.
   */
  Result& operator=(Result&&) = default;

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
  DMITIGR_JRPC_API rapidjson::Value::AllocatorType& allocator() override;

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
  friend Response;

  rapidjson::Document rep_{rapidjson::Type::kObjectType};

  explicit Result(rapidjson::Value id);
  explicit Result(rapidjson::Document rep);

  rapidjson::Value& data__();
  bool is_invariant_ok() const;
};

} // namespace dmitigr::jrpc

#ifdef DMITIGR_JRPC_HEADER_ONLY
#include "dmitigr/jrpc/result.cpp"
#endif

#endif  // DMITIGR_JRPC_RESULT_HPP
