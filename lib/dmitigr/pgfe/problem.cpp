// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#include "dmitigr/pgfe/basics.hpp"
#include "dmitigr/pgfe/problem.hpp"
#include "dmitigr/pgfe/std_system_error.hpp"
#include "dmitigr/pgfe/util.hpp"
#include <dmitigr/base/debug.hpp>

#include <stdexcept>

namespace dmitigr::pgfe::detail {

/// The generic implementation of Problem.
template<class ProblemDerived>
class basic_Problem final : public ProblemDerived {
  static_assert(std::is_base_of_v<Problem, ProblemDerived>);
public:
  /// Default-constructible.
  basic_Problem() = default;

  /// The constructor.
  basic_Problem(std::string severity_localized,
    std::optional<std::string> severity_non_localized,
    std::string sqlstate,
    std::string brief,
    std::optional<std::string> detail,
    std::optional<std::string> hint,
    std::optional<std::string> query_position,
    std::optional<std::string> internal_query_position,
    std::optional<std::string> internal_query,
    std::optional<std::string> context,
    std::optional<std::string> schema_name,
    std::optional<std::string> table_name,
    std::optional<std::string> column_name,
    std::optional<std::string> data_type_name,
    std::optional<std::string> constraint_name,
    std::optional<std::string> source_file,
    std::optional<std::string> source_line,
    std::optional<std::string> source_function)
  : severity_localized_(std::move(severity_localized))
  , severity_non_localized_(std::move(severity_non_localized))
  , sqlstate_(std::move(sqlstate))
  , brief_(std::move(brief))
  , detail_(std::move(detail))
  , hint_(std::move(hint))
  , query_position_(std::move(query_position))
  , internal_query_position_(std::move(internal_query_position))
  , internal_query_(std::move(internal_query))
  , context_(std::move(context))
  , schema_name_(std::move(schema_name))
  , table_name_(std::move(table_name))
  , column_name_(std::move(column_name))
  , data_type_name_(std::move(data_type_name))
  , constraint_name_(std::move(constraint_name))
  , source_file_(std::move(source_file))
  , source_line_(std::move(source_line))
  , source_function_(std::move(source_function))
  {
    DMITIGR_ASSERT(is_invariant_ok());
  }

  std::unique_ptr<Problem> to_problem() const override
  {
    return std::make_unique<basic_Problem>(*this);
  }

  std::error_code code() const noexcept override
  {
    return ProblemDerived::code();
  }

  Problem_severity severity() const override
  {
    return ProblemDerived::severity();
  }

  const std::string& severity_localized() const noexcept override
  {
    return severity_localized_;
  }

  const std::string& severity_non_localized() const override
  {
    if (severity_non_localized_)
      return *severity_non_localized_;
    else
      throw std::runtime_error{"severity_non_localized is not presents in"
          " reports generated by the PostgreSQL servers of versions prior to 9.6"};
  }

  const std::string& sqlstate() const noexcept override
  {
    return sqlstate_;
  }

  const std::string& brief() const noexcept override
  {
    return brief_;
  }

  const std::optional<std::string>& detail() const noexcept override
  {
    return detail_;
  }

  const std::optional<std::string>& hint() const noexcept override
  {
    return hint_;
  }

  const std::optional<std::string>& query_position() const noexcept override
  {
    return query_position_;
  }

  const std::optional<std::string>& internal_query_position() const noexcept override
  {
    return internal_query_position_;
  }

  const std::optional<std::string>& internal_query() const noexcept override
  {
    return internal_query_;
  }

  const std::optional<std::string>& context() const noexcept override
  {
    return context_;
  }

  const std::optional<std::string>& schema_name() const noexcept override
  {
    return schema_name_;
  }

  const std::optional<std::string>& table_name() const noexcept override
  {
    return table_name_;
  }

  const std::optional<std::string>& column_name() const noexcept override
  {
    return column_name_;
  }

  const std::optional<std::string>& data_type_name() const noexcept override
  {
    return data_type_name_;
  }

  const std::optional<std::string>& constraint_name() const noexcept override
  {
    return constraint_name_;
  }

  const std::optional<std::string>& source_file() const noexcept override
  {
    return source_file_;
  }

  const std::optional<std::string>& source_line() const noexcept override
  {
    return source_line_;
  }

  const std::optional<std::string>& source_function() const noexcept override
  {
    return source_function_;
  }

private:
  // The integer with the base 36 that represents the error condition "00000".
  constexpr static int min_warning_integer_code_ = 0;

  // The integer with the base 36 that represents the error condition "03000".
  constexpr static int min_error_integer_code_ = 139968;

  // The integer with the base 36 that represents the error condition "ZZZZZ".
  constexpr static int max_error_integer_code_ = 60466175;

  std::string severity_localized_;
  std::optional<std::string> severity_non_localized_;
  std::string sqlstate_;
  std::string brief_;
  std::optional<std::string> detail_;
  std::optional<std::string> hint_;
  std::optional<std::string> query_position_;
  std::optional<std::string> internal_query_position_;
  std::optional<std::string> internal_query_;
  std::optional<std::string> context_;
  std::optional<std::string> schema_name_;
  std::optional<std::string> table_name_;
  std::optional<std::string> column_name_;
  std::optional<std::string> data_type_name_;
  std::optional<std::string> constraint_name_;
  std::optional<std::string> source_file_;
  std::optional<std::string> source_line_;
  std::optional<std::string> source_function_;

  bool is_invariant_ok() const override
  {
    constexpr bool is_error = std::is_base_of_v<Error, ProblemDerived>;
    const bool mandatory_ok = !severity_localized().empty() && !sqlstate().empty();
    const bool severity_ok =
      !severity_non_localized_ ||
      (!is_error && ((severity_non_localized_ == "LOG") ||
        (severity_non_localized_ == "INFO") ||
        (severity_non_localized_ == "DEBUG") ||
        (severity_non_localized_ == "NOTICE") ||
        (severity_non_localized_ == "WARNING")))
      ||
      (is_error && ((severity_non_localized_ == "ERROR") ||
        (severity_non_localized_ == "FATAL") ||
        (severity_non_localized_ == "PANIC")));

    /*
     * Note: Error with SQLSTATE codes of classes 00, 01, 02
     * (which correspond to warnings, not errors) are legal.
     */
    const int value = code().value();
    const bool code_ok = (min_warning_integer_code_ <= value && value <= max_error_integer_code_);
    const bool problemderived_ok = ProblemDerived::is_invariant_ok();
    return mandatory_ok && severity_ok && code_ok && problemderived_ok;
  }
};

} // namespace dmitigr::pgfe::detail

namespace dmitigr::pgfe {

inline std::error_code Problem::code() const
{
  const int code_integer = sqlstate_string_to_int(sqlstate());
  return std::error_code(code_integer, server_error_category());
}

inline Problem_severity Problem::severity() const
{
  Problem_severity result{};
  const auto severity_string = severity_non_localized();
  if (severity_string == "LOG")
    result = Problem_severity::log;
  else if (severity_string == "INFO")
    result = Problem_severity::info;
  else if (severity_string == "DEBUG")
    result = Problem_severity::debug;
  else if (severity_string == "NOTICE")
    result = Problem_severity::notice;
  else if (severity_string == "WARNING")
    result = Problem_severity::warning;
  else if (severity_string == "ERROR")
    result = Problem_severity::error;
  else if (severity_string == "FATAL")
    result = Problem_severity::fatal;
  else if (severity_string == "PANIC")
    result = Problem_severity::panic;
  else
    DMITIGR_ASSERT_ALWAYS(!true);
  return result;
}

} // namespace dmitigr::pgfe
