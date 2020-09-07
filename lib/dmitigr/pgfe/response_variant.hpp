// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#ifndef DMITIGR_PGFE_RESPONSE_VARIANT_HPP
#define DMITIGR_PGFE_RESPONSE_VARIANT_HPP

#include "dmitigr/pgfe/completion.hpp"
#include "dmitigr/pgfe/error.hpp"
#include "dmitigr/pgfe/prepared_statement.hpp"
#include "dmitigr/pgfe/row.hpp"

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
    rhs.prepared_statement_ = {};
  }

  pq_Response_variant& operator=(pq_Response_variant&& rhs) noexcept
  {
    if (this != &rhs) {
      error_ = std::move(rhs.error_);
      row_ = std::move(rhs.row_);
      completion_ = std::move(rhs.completion_);
      prepared_statement_ = rhs.prepared_statement_;

      rhs.prepared_statement_ = {};
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

  pq_Response_variant(Prepared_statement* const prepared_statement) noexcept
    : prepared_statement_{prepared_statement}
  {}

  pq_Response_variant& operator=(Error&& error) noexcept
  {
    reset();
    error_ = std::move(error);
    return *this;
  }

  pq_Response_variant& operator=(Row&& row) noexcept
  {
    reset();
    row_ = std::move(row);
    return *this;
  }

  pq_Response_variant& operator=(Completion&& completion) noexcept
  {
    reset();
    completion_ = std::move(completion);
    return *this;
  }

  pq_Response_variant& operator=(Prepared_statement* const prepared_statement) noexcept
  {
    reset();
    prepared_statement_ = prepared_statement;
    return *this;
  }

  Error& error() noexcept
  {
    return error_;
  }

  const Error& error() const noexcept
  {
    return error_;
  }

  Error release_error() noexcept
  {
    return std::move(error_);
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

  Completion& completion() noexcept
  {
    return completion_;
  }

  const Completion& completion() const noexcept
  {
    return completion_;
  }

  Completion release_completion() noexcept
  {
    return std::move(completion_);
  }

  Prepared_statement* prepared_statement() const noexcept
  {
    return prepared_statement_;
  }

  const Response* response() const noexcept
  {
    if (row_)
      return &row_;
    else if (completion_)
      return &completion_;
    else if (prepared_statement_)
      return prepared_statement_;
    else if (error_)
      return &error_;
    else
      return nullptr;
  }

  std::unique_ptr<Response> release_response()
  {
    if (auto r = release_row())
      return std::make_unique<Row>(std::move(row_));
    else if (auto r = release_completion())
      return std::make_unique<Completion>(std::move(completion_));
    else if (auto r = release_error())
      return std::make_unique<Error>(std::move(error_));
    else
      return nullptr; // prepared statement cannot be released
  }

  explicit operator bool() noexcept
  {
    return static_cast<bool>(response());
  }

  void reset() noexcept
  {
    error_ = {}, row_ = {}, completion_ = {}, prepared_statement_ = {};
  }

private:
  Error error_;
  Row row_;
  Completion completion_;
  Prepared_statement* prepared_statement_{};
};

} // namespace dmitigr::pgfe::detail

#endif  // DMITIGR_PGFE_RESPONSE_VARIANT_HPP
