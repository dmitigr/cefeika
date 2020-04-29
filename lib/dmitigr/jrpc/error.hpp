// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#ifndef DMITIGR_JRPC_ERROR_HPP
#define DMITIGR_JRPC_ERROR_HPP

#include "dmitigr/jrpc/basics.hpp"
#include "dmitigr/jrpc/response_dfn.hpp"
#include "dmitigr/jrpc/std_system_error.hpp"
#include "dmitigr/rajson/conversions.hpp"

#include <memory>

namespace dmitigr::jrpc {

/**
 * @brief An error response.
 */
class Error final : public Response, public std::system_error {
public:
  /**
   * @brief Constructs an instance with code of Server_errc::generic_error,
   * with null ID, and empty message.
   */
  DMITIGR_JRPC_API Error();

  /**
   * @brief The constructor.
   */
  DMITIGR_JRPC_API Error(std::error_code code, Null id,
    const std::string& message = {});

  /**
   * @overload
   */
  DMITIGR_JRPC_API Error(std::error_code code, int id,
    const std::string& message = {});

  /**
   * @overload
   */
  DMITIGR_JRPC_API Error(std::error_code code, std::string_view id,
    const std::string& message = {});

  /**
   * @brief Exchange the contents of this request with `other`.
   */
  DMITIGR_JRPC_API void swap(Error& other);

  /**
   * @see Response::jsonrpc().
   */
  DMITIGR_JRPC_API std::string_view jsonrpc() const override;

  /**
   * @see Response::id().
   */
  DMITIGR_JRPC_API const rapidjson::Value& id() const override;

  /**
   * @see Response::to_string().
   */
  DMITIGR_JRPC_API std::string to_string() const override;

  /**
   * @returns A Primitive or Structured value that contains additional
   * information about the error, or `nullptr` if omitted.
   */
  DMITIGR_JRPC_API const rapidjson::Value* data() const;

  /**
   * @brief Sets the additional information about the error.
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

  /**
   * @see Response::allocator().
   */
  DMITIGR_JRPC_API rapidjson::Value::AllocatorType& allocator() const override;

private:
  friend Request;
  friend Response;

  std::shared_ptr<rapidjson::Document> rep_;

  bool is_invariant_ok() const;
  const rapidjson::Value& error() const;
  rapidjson::Value& error();

  Error(std::error_code code, rapidjson::Value&& id, const std::string& message = {});
  Error(std::error_code code, const rapidjson::Value& id, const std::string& message = {});

  Error(std::error_code code, const std::string& message, std::shared_ptr<rapidjson::Document> rep);
  Error(std::error_code code, const std::string& message = {});
  void init__(rapidjson::Value&& id, const std::string& message);
};

} // namespace dmitigr::jrpc

#ifdef DMITIGR_JRPC_HEADER_ONLY
#include "dmitigr/jrpc/error.cpp"
#endif

#endif  // DMITIGR_JRPC_ERROR_HPP
