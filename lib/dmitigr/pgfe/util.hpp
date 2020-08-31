// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#ifndef DMITIGR_PGFE_UTIL_HPP
#define DMITIGR_PGFE_UTIL_HPP

#include "dmitigr/pgfe/dll.hpp"
#include "dmitigr/pgfe/types_fwd.hpp"

#include <chrono>
#include <optional>
#include <string>

namespace dmitigr::pgfe {

/// @returns The case-folded and double-quote processed SQL identifier.
DMITIGR_PGFE_API std::string unquote_identifier(std::string_view identifier);

namespace detail {

/// A wrapper around net::poll().
Socket_readiness poll_sock(const int socket, const Socket_readiness mask,
  const std::optional<std::chrono::milliseconds> timeout);

} // namespace detail
} // namespace dmitigr::pgfe

#ifdef DMITIGR_PGFE_HEADER_ONLY
#include "dmitigr/pgfe/util.cpp"
#endif

#endif  // DMITIGR_PGFE_UTIL_HPP
