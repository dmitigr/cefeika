// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or uuid.hpp

#ifndef DMITIGR_UUID_UUID_HPP
#define DMITIGR_UUID_UUID_HPP

#include "dmitigr/uuid/dll.hpp"
#include "dmitigr/uuid/types_fwd.hpp"

#include <memory>
#include <string>

namespace dmitigr::uuid {

/**
 * @brief An UUID.
 */
class Uuid {
public:
  /**
   * @brief The destructor.
   */
  virtual ~Uuid() = default;

  /**
   * Constructs the random UUID (version 4).
   *
   * @remarks Be sure to seed the pseudo-random number generator with
   * rand::seed() before making the instance of this class.
   */
  static DMITIGR_UUID_API std::unique_ptr<Uuid> make();

  /**
   * @returns The string representation of the UUID.
   */
  virtual std::string to_string() const = 0;

private:
  friend detail::iUuid;

  Uuid() = default;
};

} // namespace dmitigr::uuid

#ifdef DMITIGR_UUID_HEADER_ONLY
#include "dmitigr/uuid/uuid.cpp"
#endif

#endif  // DMITIGR_UUID_UUID_HPP
