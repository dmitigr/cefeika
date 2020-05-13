// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or jrpc.hpp

#ifndef DMITIGR_JRPC_BASICS_HPP
#define DMITIGR_JRPC_BASICS_HPP

namespace dmitigr::jrpc {

/**
 * @brief Represents null.
 */
struct Null final {};

/**
 * @brief A constant of type `Null` that is used to indicate null state.
 */
inline constexpr Null null;

/**
 * @brief Parameters notation.
 */
enum class Parameters_notation {
  /** Positional notation. */
  positional,

  /** Named notation. */
  named
};

/**
 * @returns The literal representation of the `value`, or `nullptr`
 * if `value` does not corresponds to any value defined by Parameters_notation.
 */
constexpr const char* to_literal(const Parameters_notation value)
{
  switch (value) {
  case Parameters_notation::positional:
    return "positional";
  case Parameters_notation::named:
    return "named";
  }
  return nullptr;
}

} // namespace dmitigr::jrpc

#endif  // DMITIGR_JRPC_BASICS_HPP
