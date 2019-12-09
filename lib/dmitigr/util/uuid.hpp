// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or util.hpp

#ifndef DMITIGR_UTIL_UUID_HPP
#define DMITIGR_UTIL_UUID_HPP

#include "dmitigr/util/dll.hpp"
#include "dmitigr/util/types_fwd.hpp"

#include <memory>
#include <string>

namespace dmitigr::uuid {

/**
 * @brief An UUID.
 */
class Uuid {
public:
  /**
   * Constructs the random UUID (version 4).
   *
   * @remarks Be sure to seed the pseudo-random number generator with
   * rand::seed() before making the instance of this class.
   */
  static DMITIGR_UTIL_API std::unique_ptr<Uuid> make();

  /**
   * @returns The string representation of the UUID.
   */
  virtual std::string to_string() const = 0;

private:
  friend detail::iUuid;

  Uuid() = default;
};

} // namespace dmitigr::uuid

#ifdef DMITIGR_UTIL_HEADER_ONLY
#include "dmitigr/util/uuid.cpp"
#endif

#endif  // DMITIGR_UTIL_UUID_HPP
