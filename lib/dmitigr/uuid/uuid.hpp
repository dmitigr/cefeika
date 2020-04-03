// -*- C++ -*-
// Copyright (C) 2020 Dmitry Igrishin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
// Dmitry Igrishin
// dmitigr@gmail.com

#ifndef DMITIGR_UUID_UUID_HPP
#define DMITIGR_UUID_UUID_HPP

#include "dmitigr/uuid/dll.hpp"
#include "dmitigr/uuid/types_fwd.hpp"
#include "dmitigr/uuid/version.hpp"

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
