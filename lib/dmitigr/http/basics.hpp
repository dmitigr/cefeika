// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_BASICS_HPP
#define DMITIGR_HTTP_BASICS_HPP

#include <dmitigr/base/debug.hpp>

#include <optional>
#include <string_view>

namespace dmitigr::http {

/**
 * @ingroup headers
 *
 * @brief A value of "SameSite" cookie attribute.
 */
enum class Same_site { strict, lax };

/**
 * @ingroup headers
 *
 * @returns The result of conversion of `str` to the value of type Same_site.
 *
 * @remarks The value of `str` is case-sensitive.
 */
inline Same_site to_same_site(const std::string_view str)
{
  if (str == "Strict")
    return Same_site::strict;
  else if (str == "Lax")
    return Same_site::lax;
  else
    DMITIGR_THROW_REQUIREMENT_VIOLATED(str == "Strict" || str == "Lax", std::invalid_argument);
}

/**
 * @ingroup headers
 *
 * @returns The result of conversion of `ss` to the instance of type `std::string`.
 */
inline std::string to_string(const Same_site ss)
{
  switch (ss) {
  case Same_site::strict: return "Strict";
  case Same_site::lax: return "Lax";
  }
  DMITIGR_ASSERT_ALWAYS(!true);
}

// -----------------------------------------------------------------------------

/// A HTTP request method.
enum class Method {
  get,
  head,
  post,
  put,
  del,
  connect,
  options,
  trace
};

/**
 * @returns The literal representation of the `method`, or `nullptr`
 * if `method` does not corresponds to any value defined by Method.
 */
constexpr const char* to_literal(const Method method)
{
  switch (method) {
  case Method::get: return "GET";
  case Method::head: return "HEAD";
  case Method::post: return "POST";
  case Method::put: return "PUT";
  case Method::del: return "DELETE";
  case Method::connect: return "CONNECT";
  case Method::options: return "OPTIONS";
  case Method::trace: return "TRACE";
  }
  return nullptr;
}

/**
 * @ingroup headers
 *
 * @returns The result of conversion of `str` to the value of type Method.
 *
 * @remarks The value of `str` is case-sensitive.
 */
inline std::optional<Method> to_method(const std::string_view str)
{
  if (str == "GET") return Method::get;
  else if (str == "HEAD") return Method::head;
  else if (str == "POST") return Method::post;
  else if (str == "PUT") return Method::put;
  else if (str == "DELETE") return Method::del;
  else if (str == "CONNECT") return Method::connect;
  else if (str == "OPTIONS") return Method::options;
  else if (str == "TRACE") return Method::trace;
  else return std::nullopt;
}

} // namespace dmitigr::http

#endif  // DMITIGR_HTTP_BASICS_HPP
