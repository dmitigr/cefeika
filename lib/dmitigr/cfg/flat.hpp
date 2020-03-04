// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or cfg.hpp

#ifndef DMITIGR_CFG_FLAT_HPP
#define DMITIGR_CFG_FLAT_HPP

#include "dmitigr/cfg/dll.hpp"
#include "dmitigr/cfg/types_fwd.hpp"
#include "dmitigr/str.hpp"
#include "dmitigr/util/filesystem.hpp"

#include <map>
#include <memory>
#include <optional>
#include <string>

namespace dmitigr::cfg {

/**
 * @brief A flat configuration store.
 *
 * Each line of the configuration store can be written in form:
 *
 *   - param1=one;
 *   - param123='one two  three';
 *   - param1234='one \'two three\' four'.
 */
class Flat {
public:
  /**
   * @returns A new instance of this class.
   */
  static DMITIGR_CFG_API std::unique_ptr<Flat> make(const std::filesystem::path& path);

  /**
   * @returns The string parameter named by `name` if it presents, or
   * `std::nullopt` otherwise.
   */
  virtual const std::optional<std::string>& string_parameter(const std::string& name) const = 0;

  /**
   * @returns The boolean parameter named by `name` if it presents, or
   * `std::nullopt` otherwise.
   */
  virtual std::optional<bool> boolean_parameter(const std::string& name) const = 0;

  /**
   * @returns The parameter map.
   */
  virtual const std::map<std::string, std::optional<std::string>>& parameters() const = 0;

private:
  friend detail::iFlat;

  Flat() = default;
};

} // namespace dmitigr::cfg

#ifdef DMITIGR_CFG_HEADER_ONLY
#include "dmitigr/cfg/flat.cpp"
#endif

#endif  // DMITIGR_CFG_FLAT_HPP
