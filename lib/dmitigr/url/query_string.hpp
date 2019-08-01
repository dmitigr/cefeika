// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or url.hpp

#ifndef DMITIGR_URL_QUERY_STRING_HPP
#define DMITIGR_URL_QUERY_STRING_HPP

#include "dmitigr/url/dll.hpp"
#include "dmitigr/url/types_fwd.hpp"

#include <memory>
#include <optional>
#include <string>

namespace dmitigr::url {

/**
 * @brief An URL query string parameter.
 */
class Query_string_parameter {
public:
  /**
   * @returns The parameter name.
   */
  virtual const std::string& name() const = 0;

  /**
   * @brief Sets the name of the parameter.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  virtual void set_name(std::string name) = 0;

  /**
   * @returns The parameter value.
   */
  virtual const std::optional<std::string>& value() const = 0;

  /**
   * @brief Sets the value of the parameter.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  virtual void set_value(std::optional<std::string> value) = 0;

private:
  friend detail::iQuery_string_parameter;

  Query_string_parameter() = default;
};

/**
 * @brief An URL query string.
 *
 * @remarks Since several parameters can be named equally, `offset` can be
 * specified as the starting lookup index in the corresponding methods.
 */
class Query_string {
public:
  /**
   * @brief The alias of Query_string_parameter.
   */
  using Parameter = Query_string_parameter;

  /**
   * @brief The destructor.
   */
  virtual ~Query_string() = default;

  /// @name Constructors
  /// @{

  /**
   * @brief Constructs the object by parsing the query string `input`.
   *
   * Examples of valid input are:
   *
   *   1. param1=value1&param2=2
   *   2. param1=value1&param2=
   *   3. param1=value1&param2
   *   4. name=%D0%B4%D0%B8%D0%BC%D0%B0&age=35
   *
   * Note, the value of parameter "param2" will be parsed as: "2" in
   * case 1, "" (empty string) in case 2 and `std::nullopt` in case 3.
   *
   * @returns A new instance of this class.
   *
   * @param input - unparsed (possibly percent-encoded) query string
   */
  static DMITIGR_URL_API std::unique_ptr<Query_string> make(std::string_view input);

  /// @}

  /**
   * @returns The number of parameters.
   */
  virtual std::size_t parameter_count() const = 0;

  /**
   * @returns The parameter index if `has_parameter(name, offset)`, or
   * `std::nullopt` otherwise.
   */
  virtual std::optional<std::size_t> parameter_index(std::string_view name, std::size_t offset = 0) const = 0;

  /**
   * @returns The parameter index.
   *
   * @par Requires
   * `has_parameter(name, offset)`.
   */
  virtual std::size_t parameter_index_throw(std::string_view name, std::size_t offset = 0) const = 0;

  /**
   * @returns The parameter.
   *
   * @par Requires
   * `(index < parameter_count())`.
   */
  virtual const Parameter* parameter(std::size_t index) const = 0;

  /**
   * @overload
   */
  virtual Parameter* parameter(std::size_t index) = 0;

  /**
   * @overload
   *
   * @par Requires
   * `has_parameter(name, offset)`.
   */
  virtual const Parameter* parameter(std::string_view name, std::size_t offset = 0) const = 0;

  /**
   * @overload
   */
  virtual Parameter* parameter(std::string_view name, std::size_t offset = 0) = 0;

  /**
   * @returns `true` if the parameter named by `name` is presents, or
   * `false` otherwise.
   */
  virtual bool has_parameter(std::string_view name, std::size_t offset = 0) const = 0;

  /**
   * @returns `(parameter_count() > 0)`.
   */
  virtual bool has_parameters() const = 0;

  /**
   * @brief Appends the parameter to this query string.
   *
   * @param name - parameter name to set;
   * @param value - parameter value to set.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  virtual void append_parameter(std::string name, std::optional<std::string> value) = 0;

  /**
   * @brief Removes parameter from this query string.
   *
   * @par Requires
   * `(index < parameter_count())`.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  virtual void remove_parameter(std::size_t index) = 0;

  /**
   * @overload
   *
   * @par Effects
   * `!has_parameter(name, offset)`.
   */
  virtual void remove_parameter(std::string_view name, std::size_t offset = 0) = 0;

  /// @name Conversions
  /// @{

  /**
   * @returns The result of conversion of this instance to the instance of type `std::string`.
   */
  virtual std::string to_string() const = 0;

  /// @}

private:
  friend detail::iQuery_string;

  Query_string() = default;
};

} // namespace dmitigr::url

#ifdef DMITIGR_URL_HEADER_ONLY
#include "dmitigr/url/query_string.cpp"
#endif

#endif  // DMITIGR_URL_QUERY_STRING_HPP
