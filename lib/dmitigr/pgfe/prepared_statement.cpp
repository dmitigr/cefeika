// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#include "dmitigr/pgfe/prepared_statement.hpp"
#include "dmitigr/pgfe/connection.hpp"
#include "dmitigr/pgfe/exceptions.hpp"
#include "dmitigr/pgfe/sql_string.hpp"

#include <algorithm>

namespace dmitigr::pgfe {

DMITIGR_PGFE_INLINE std::size_t Prepared_statement::positional_parameter_count() const
{
  const auto b = cbegin(parameters_);
  const auto e = cend(parameters_);
  const auto i = std::find_if_not(b, e, [](const auto& p) { return p.name.empty(); });
  return i - b;
}

// std::visit is slow to compile
DMITIGR_PGFE_INLINE std::optional<std::uint_fast32_t> Prepared_statement::parameter_type_oid(const std::size_t index) const noexcept
{
  assert(index < parameter_count());
  if (is_described()) {
    return std::visit(
      [index](const auto& descr) -> std::uint_fast32_t
      {
        using T = std::decay_t<decltype (descr)>;
        if constexpr (std::is_same_v<T, detail::pq::Result>)
          return descr.ps_param_type_oid(static_cast<int>(index));
        else if constexpr (std::is_same_v<T, Row_info>)
          return descr.pq_result_.ps_param_type_oid(static_cast<int>(index));
      }, *description_);
  } else
    return {};
}

// std::visit is slow to compile
DMITIGR_PGFE_INLINE const Row_info* Prepared_statement::row_info() const noexcept
{
  if (is_described()) {
    return std::visit(
      [](const auto& descr) -> const Row_info*
      {
        using T = std::decay_t<decltype (descr)>;
        if constexpr (std::is_same_v<T, detail::pq::Result>)
          return nullptr;
        else if constexpr (std::is_same_v<T, Row_info>)
          return &descr;
      }, *description_);
  } else
    return nullptr;
}

DMITIGR_PGFE_INLINE Prepared_statement::Prepared_statement(std::string name,
  Connection* const connection, const Sql_string* const preparsed)
  : name_(std::move(name))
  , preparsed_(static_cast<bool>(preparsed))
{
  init_connection__(connection);

  if (preparsed_) {
    const std::size_t pc = preparsed->parameter_count();
    parameters_.resize(pc);
    for (std::size_t i = preparsed->positional_parameter_count(); i < pc; ++i)
      parameters_[i].name = preparsed->parameter_name(i);
  } else
    parameters_.reserve(8);

  assert(is_invariant_ok());
}

DMITIGR_PGFE_INLINE Prepared_statement::Prepared_statement(std::string name,
  Connection* const connection, const std::size_t parameters_count)
  : name_(std::move(name))
  , parameters_(parameters_count)
{
  init_connection__(connection);
  assert(is_invariant_ok());
}

DMITIGR_PGFE_INLINE void Prepared_statement::init_connection__(Connection* const connection)
{
  assert(connection && connection->session_start_time());
  connection_ = connection;
  session_start_time_ = *connection_->session_start_time();
  result_format_ = connection_->result_format();
}

DMITIGR_PGFE_INLINE bool Prepared_statement::is_invariant_ok() const noexcept
{
  const bool params_ok = (parameter_count() <= maximum_parameter_count());
  const bool preparsed_ok = is_preparsed() || !has_named_parameters();
  const bool session_ok = (session_start_time_ == connection_->session_start_time());
  const bool parameterizable_ok = Parameterizable::is_invariant_ok();
  return params_ok && preparsed_ok && session_ok && parameterizable_ok;
}

DMITIGR_PGFE_INLINE void Prepared_statement::execute_async()
{
  assert(connection()->is_ready_for_async_request());

  // All values are NULLs. (Can throw.)
  const int param_count = static_cast<int>(parameter_count());
  std::vector<const char*> values(param_count, nullptr);
  std::vector<int> lengths(param_count, 0);
  std::vector<int> formats(param_count, 0);

  connection_->requests_.push(Connection::Request_id::execute); // can throw
  try {
    // Prepare the input for libpq.
    for (int i = 0; i < param_count; ++i) {
      if (const Data* const d = parameter(i)) {
        values[i] = d->bytes();
        lengths[i] = static_cast<int>(d->size());
        formats[i] = detail::pq::to_int(d->format());
      }
    }
    const int result_format = detail::pq::to_int(result_format_);

    const int send_ok = ::PQsendQueryPrepared(connection_->conn(), name_.c_str(),
      param_count, values.data(), lengths.data(), formats.data(), result_format);
    if (!send_ok)
      throw std::runtime_error(connection_->error_message());

    const auto set_ok = ::PQsetSingleRowMode(connection_->conn());
    if (!set_ok)
      throw std::runtime_error{"cannot switch to single-row mode"};
  } catch (...) {
    connection_->requests_.pop(); // rollback
    throw;
  }

  assert(is_invariant_ok());
}

DMITIGR_PGFE_INLINE void Prepared_statement::execute()
{
  assert(connection()->is_ready_for_request());
  execute_async();
  assert(is_invariant_ok());
}

DMITIGR_PGFE_INLINE void Prepared_statement::describe_async()
{
  connection_->describe_statement_async(name_);
  assert(is_invariant_ok());
}

DMITIGR_PGFE_INLINE void Prepared_statement::describe()
{
  connection_->describe_statement(name_);
  assert(is_invariant_ok());
}

DMITIGR_PGFE_INLINE std::size_t Prepared_statement::parameter_index__(const std::string& name) const noexcept
{
  const auto b = cbegin(parameters_);
  const auto e = cend(parameters_);
  const auto i = std::find_if(b, e, [&name](const auto& p) { return p.name == name; });
  return i - b;
}

} // namespace dmitigr::pgfe
