// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ttpl.hpp

#ifndef DMITIGR_TTPL_LOGIC_LESS_TEMPLATE_HPP
#define DMITIGR_TTPL_LOGIC_LESS_TEMPLATE_HPP

#include "dmitigr/ttpl/dll.hpp"
#include "dmitigr/ttpl/types_fwd.hpp"

#include <memory>
#include <optional>
#include <string>

namespace dmitigr::ttpl {

/**
 * @brief A logic less text template parameter.
 */
class Logic_less_template_parameter {
public:
  /**
   * @returns The parameter name.
   */
  virtual const std::string& name() const = 0;

  /**
   * @returns The parameter value.
   */
  virtual const std::optional<std::string>& value() const = 0;

  /**
   * @brief Sets the value of parameter.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  virtual void set_value(std::optional<std::string> value) = 0;

private:
  friend detail::iLogic_less_template_parameter;

  Logic_less_template_parameter() = default;
};

/**
 * @brief A logic less text template.
 *
 * This is a tiny and very simple template engine.
 */
class Logic_less_template {
public:
  /**
   * @brief The alias of Logic_less_template_parameter.
   */
  using Parameter = Logic_less_template_parameter;

  /**
   * @brief The destructor.
   */
  virtual ~Logic_less_template() = default;

  /// @name Constructors
  /// @{

  /**
   * @brief Constructs the object by parsing the `input`.
   *
   * The `input` may contain parameters which can be binded with the values
   * by using Parameter::set_value() method. The parameter name *must* be
   * surrounded with doubled opening and closing curly brackets and *exactly one*
   * space on both sides, or otherwise, it will be treated as the regular text
   * and will be outputted as is. The name of parameter can only consist of
   * alphanumerics, the underscore character ("_") and the hyphen character ("-").
   *
   * Examples of valid input:
   *
   *   1. Hello {{ name }}!
   *   2. Hello {{name}}!
   *   3. Hello {{  name}}!
   *
   *   The input in the example 1 contain one parameter "name" which can be
   *   bound with a value, while the input in the examples 2 and 3 has no
   *   parameters and will be outputted as is.
   *
   * @returns A new instance of this class.
   */
  static DMITIGR_TTPL_API std::unique_ptr<Logic_less_template> make(std::string_view input = {});

  /**
   * @returns The copy of this instance.
   */
  virtual std::unique_ptr<Logic_less_template> to_logic_less_template() const = 0;

  /// @}

  /**
   * @returns The number of parameters.
   */
  virtual std::size_t parameter_count() const = 0;

  /**
   * @returns The parameter index if `has_parameter(name)`, or
   * `std::nullopt` otherwise.
   */
  virtual std::optional<std::size_t> parameter_index(std::string_view name) const = 0;

  /**
   * @returns The parameter index.
   *
   * @par Requires
   * `has_parameter(name)`.
   */
  virtual std::size_t parameter_index_throw(std::string_view name) const = 0;

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
   * `has_parameter(name)`.
   */
  virtual const Parameter* parameter(std::string_view name) const = 0;

  /**
   * @overload
   */
  virtual Parameter* parameter(std::string_view name) = 0;

  /**
   * @returns `true` if the parameter named by `name` is presents, or
   * `false` otherwise.
   */
  virtual bool has_parameter(std::string_view name) const = 0;

  /**
   * @returns `(parameter_count() > 0)`
   */
  virtual bool has_parameters() const = 0;

  /**
   * @returns `true` if this instance has the parameter with the index `i`
   * such that `(parameter(i)->value() == std::nullopt)`, or `false` otherwise.
   *
   * @see parameter().
   */
  virtual bool has_unset_parameters() const = 0;

  /**
   * @brief Replaces the parameter named by the `name` with the specified
   * `replacement`.
   *
   * @par Requires
   * `(has_parameter(name) && replacement && replacement != this)`.
   *
   * @par Effects
   * This instance contains the given `replacement` instead of the parameter
   * named by the `name`. Parameter indexes likely changed.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @see has_parameter().
   */
  virtual void replace_parameter(std::string_view name, const Logic_less_template* replacement) = 0;

  /**
   * @overload
   */
  virtual void replace_parameter(std::string_view name, std::string_view replacement) = 0;

  /// @name Conversions
  /// @{

  /**
   * @returns The result of conversion of this instance to the instance of
   * type `std::string`.
   */
  virtual std::string to_string() const = 0;

  /**
   * @returns The output string.
   */
  virtual std::string to_output() const = 0;

  /// @}
private:
  friend detail::iLogic_less_template;

  Logic_less_template() = default;
};

} // namespace dmitigr::ttpl

#ifdef DMITIGR_TTPL_HEADER_ONLY
#include "dmitigr/ttpl/logic_less_template.cpp"
#endif

#endif  // DMITIGR_TTPL_LOGIC_LESS_TEMPLATE_HPP
