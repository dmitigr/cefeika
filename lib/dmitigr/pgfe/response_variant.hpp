// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#ifndef DMITIGR_PGFE_RESPONSE_VARIANT_HPP
#define DMITIGR_PGFE_RESPONSE_VARIANT_HPP

#include "dmitigr/pgfe/completion.hpp"
#include "dmitigr/pgfe/error.hpp"
#include "dmitigr/pgfe/prepared_statement_dfn.hpp"
#include "dmitigr/pgfe/row.hpp"

#include <optional>

namespace dmitigr::pgfe::detail {

class pq_Response_variant final {
public:
  pq_Response_variant() = default;

  pq_Response_variant(pq_Response_variant&& rhs) noexcept
    : error_{std::move(rhs.error_)}
    , row_{std::move(rhs.row_)}
    , completion_{std::move(rhs.completion_)}
    , prepared_statement_{rhs.prepared_statement_}
  {
    rhs.error_.reset(), rhs.completion_.reset(), rhs.prepared_statement_ = {};
  }

  pq_Response_variant& operator=(pq_Response_variant&& rhs) noexcept
  {
    if (this != &rhs) {
      error_ = std::move(rhs.error_);
      row_ = std::move(rhs.row_);
      completion_ = std::move(rhs.completion_);
      prepared_statement_ = rhs.prepared_statement_;

      rhs.error_.reset(), rhs.completion_.reset(), rhs.prepared_statement_ = {};
    }
    return *this;
  }

  pq_Response_variant(Error&& error) noexcept
    : error_{std::move(error)}
  {}

  pq_Response_variant(Row&& row) noexcept
    : row_{std::move(row)}
  {}

  pq_Response_variant(Completion&& completion) noexcept
    : completion_{std::move(completion)}
  {}

  pq_Response_variant(pq_Prepared_statement* const prepared_statement) noexcept
    : prepared_statement_{prepared_statement}
  {}

  pq_Response_variant& operator=(Error&& error) noexcept
  {
    error_ = std::move(error);
    row_ = {}, completion_.reset(), prepared_statement_ = {};
    return *this;
  }

  pq_Response_variant& operator=(Row&& row) noexcept
  {
    row_ = std::move(row);
    error_.reset(), completion_.reset(), prepared_statement_ = {};
    return *this;
  }

  pq_Response_variant& operator=(Completion&& completion) noexcept
  {
    completion_ = std::move(completion);
    error_.reset(), row_ = {}, prepared_statement_ = {};
    return *this;
  }

  pq_Response_variant& operator=(pq_Prepared_statement* const prepared_statement) noexcept
  {
    prepared_statement_ = prepared_statement, error_.reset(), row_ = {}, completion_.reset();
    return *this;
  }

  std::optional<Error>& error() noexcept
  {
    return error_;
  }

  const std::optional<Error>& error() const noexcept
  {
    return error_;
  }

  std::optional<Error> release_error() noexcept
  {
    auto r = std::move(error_);
    error_.reset();
    return r;
  }

  Row& row() noexcept
  {
    return row_;
  }

  const Row& row() const noexcept
  {
    return row_;
  }

  Row release_row() noexcept
  {
    return std::move(row_);
  }

  std::optional<Completion>& completion() noexcept
  {
    return completion_;
  }

  const std::optional<Completion>& completion() const noexcept
  {
    return completion_;
  }

  std::optional<Completion> release_completion() noexcept
  {
    auto r = std::move(completion_);
    completion_.reset();
    return r;
  }

  pq_Prepared_statement* prepared_statement() const noexcept
  {
    return prepared_statement_;
  }

  const Response* response() const noexcept
  {
    if (row_)
      return &row_;
    else if (completion_)
      return &*completion_;
    else if (prepared_statement_)
      return prepared_statement_;
    else if (error_)
      return &*error_;
    else
      return nullptr;
  }

  std::unique_ptr<Response> release_response()
  {
    if (auto r = release_row())
      return std::make_unique<Row>(std::move(row_));
    else if (completion_) {
      auto res = std::make_unique<Completion>(std::move(*completion_));
      completion_.reset();
      return res;
    } else if (error_) {
      auto res = std::make_unique<Error>(std::move(*error_));
      error_.reset();
      return res;
    } else
      return nullptr; // prepared statement cannot be released
  }

  explicit operator bool() noexcept
  {
    return static_cast<bool>(response());
  }

  void reset() noexcept
  {
    error_.reset(), row_ = {}, completion_.reset(), prepared_statement_ = {};
  }

private:
  std::optional<Error> error_;
  Row row_;
  std::optional<Completion> completion_;
  pq_Prepared_statement* prepared_statement_{};
};

} // namespace dmitigr::pgfe::detail

#endif  // DMITIGR_PGFE_RESPONSE_VARIANT_HPP
