// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#include "dmitigr/pgfe/basics.hpp"
#include "dmitigr/pgfe/problem.hpp"
#include "dmitigr/pgfe/std_system_error.hpp"

#include <cassert>
#include <cerrno>
#include <locale>
#include <stdexcept>

namespace dmitigr::pgfe {

Problem::Problem(detail::pq::Result&& result) noexcept
  : pq_result_{std::move(result)}
{
  const int code = sqlstate_string_to_int(pq_result_.er_code());
  code_ = {code, server_error_category()};
  assert(is_invariant_ok());
}

Problem_severity DMITIGR_PGFE_INLINE Problem::severity() const noexcept
{
  const char* const s = pq_result_.er_severity_non_localized();
  return s ? to_problem_severity(std::string_view{s}) : Problem_severity{-1};
}

DMITIGR_PGFE_INLINE std::error_code Problem::min_code() noexcept
{
  return {0, server_error_category()};
}

DMITIGR_PGFE_INLINE std::error_code Problem::max_code() noexcept
{
  return {60466175, server_error_category()};
}

DMITIGR_PGFE_INLINE std::error_code Problem::min_error_code() noexcept
{
  return {139968, server_error_category()};
}

DMITIGR_PGFE_INLINE int Problem::sqlstate_string_to_int(const char* const code) noexcept
{
  if (!(code && code[5] == '\0')) {
    assert(false);
    return -1;
  }

  const std::locale l{};
  const std::string_view cod{code};
  if (!(cod.size() == 5 &&
      std::isalnum(cod[0], l) &&
      std::isalnum(cod[1], l) &&
      std::isalnum(cod[2], l) &&
      std::isalnum(cod[3], l) &&
      std::isalnum(cod[4], l))) {
    assert(false);
    return -1;
  }

  errno = 0;
  const long int result = std::strtol(cod.data(), nullptr, 36);
  assert(errno == 0);
  assert(min_code().value() <= result && result <= max_code().value());
  return result;
}

DMITIGR_PGFE_INLINE std::string Problem::sqlstate_int_to_string(const int code)
{
  if (!(min_code().value() <= code && code <= max_code().value())) {
    assert(false);
    throw std::invalid_argument{"invalid SQLSTATE code"};
  }
  return str::to_string(code, 36);
}

} // namespace dmitigr::pgfe
