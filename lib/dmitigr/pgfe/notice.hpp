// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#ifndef DMITIGR_PGFE_NOTICE_HPP
#define DMITIGR_PGFE_NOTICE_HPP

#include "dmitigr/pgfe/problem.hpp"
#include "dmitigr/pgfe/signal.hpp"

namespace dmitigr::pgfe {

/**
 * @ingroup main
 *
 * @brief An unprompted (asynchronous) notice from a PostgreSQL server.
 *
 * The notice is an information about an activity of the PostgreSQL server.
 * (For example, it might be the database administrator's commands.)
 *
 * @remarks It should not be confused with the Notification signal.
 */
class Notice final : public Signal, public Problem {
public:
  ~Notice() override
  {
    pq_result_.release(); // freed in libpq/fe-protocol3.c:pqGetErrorNotice3()
  }

  Notice() = default;

  explicit Notice(const ::PGresult* const result) noexcept
    : Problem{detail::pq::Result{const_cast< ::PGresult*>(result)}}
  {
    /*
     * In fact result is not const. So it's okay to const_cast.
     * (Allocated in libpq/fe-protocol3.c:pqGetErrorNotice3().)
     */
    assert(is_invariant_ok());
  }

private:
  bool is_invariant_ok() const noexcept override
  {
    const auto sev = severity();
    return ((static_cast<int>(sev) == -1) ||
      (sev == Problem_severity::log) ||
      (sev == Problem_severity::info) ||
      (sev == Problem_severity::debug) ||
      (sev == Problem_severity::notice) ||
      (sev == Problem_severity::warning)) && Problem::is_invariant_ok();
  }

  // TODO: copying
  // struct Rep;
  // std::unique_ptr<Rep> rep_;
};

} // namespace dmitigr::pgfe

#endif  // DMITIGR_PGFE_NOTICE_HPP
