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

#ifndef DMITIGR_MISC_PROGPAR_HPP
#define DMITIGR_MISC_PROGPAR_HPP

#include "dmitigr/misc/filesystem.hpp"

#include <algorithm>
#include <cassert>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace dmitigr::progpar {

/**
 * @brief Program parameters.
 *
 * Stores the parsed program parameters like the following:
 *   executabe [--opt1 --opt2=arg] [--] [arg1 arg2]
 *
 * Each option may have an argument which is specified after the "=" character.
 * The sequence of characters "--" indicates that the remaining parameters should
 * not be treated as options, but arguments.
 *
 * @remarks "Short" options (e.g. `-o` or `-o 1`) does not supported currently
 * and treated as arguments.
 */
class Program_parameters final {
public:
  /// The alias to represent a map of program options.
  using Option_map = std::map<std::string, std::optional<std::string>>;

  /// The alias to represent a vector of program arguments.
  using Argument_vector = std::vector<std::string>;

  /// The default constructor. (Constructs invalid instance.)
  Program_parameters() = default;

  /**
   * @brief The constructor.
   *
   * @par Requires
   * `(argc > 0 && argv && argv[0])`.
   */
  Program_parameters(const int argc, const char* const* argv)
  {
    assert(argc > 0 && argv && argv[0]);

    static const auto opt = [](const std::string_view arg)
      -> std::optional<std::pair<std::string, std::optional<std::string>>>
      {
        if (auto pos = arg.find("--"); pos == 0) {
          if (arg.size() == 2) {
            return std::make_pair(std::string{}, std::nullopt);
          } else if (pos = arg.find('=', 2); pos != std::string::npos) {
            auto name = arg.substr(2, pos - 2);
            auto value = arg.substr(pos + 1);
            return std::make_pair(std::string{name}, std::string{value});
          } else
            return std::make_pair(std::string{arg.substr(2)}, std::nullopt);
        } else
          return std::nullopt;
      };

    executable_path_.assign(argv[0]);

    if (argc == 1)
      return;

    int argi = 1;

    // Collecting options
    for (; argi < argc; ++argi) {
      if (auto o = opt(argv[argi])) {
        if (o->first.empty()) {
          ++argi;
          break;
        } else
          options_[std::move(o->first)] = std::move(o->second);
      } else
        break;
    }

    // Collecting arguments
    for (; argi < argc; ++argi)
      arguments_.emplace_back(argv[argi]);

    assert(is_valid());
  }

  /**
   * @brief The constructor.
   *
   * @par Requires
   * `(!executable_path.empty())`.
   */
  explicit Program_parameters(std::filesystem::path executable_path,
    Option_map options = {}, Argument_vector arguments = {})
    : executable_path_{std::move(executable_path)}
    , options_{std::move(options)}
    , arguments_{std::move(arguments)}
  {
    assert(!executable_path_.empty());
    assert(is_valid());
  }

  /// @returns `false` if this instance is default-constructed.
  bool is_valid() const
  {
    return !executable_path_.empty();
  }

  /// @returns The executable path.
  const std::filesystem::path& executable_path() const
  {
    return executable_path_;
  }

  /// @returns The map of options.
  const Option_map& options() const
  {
    return options_;
  }

  /// @returns The vector of arguments.
  const Argument_vector& arguments() const
  {
    return arguments_;
  }

  /**
   * @returns An option argument, or `nullptr` if option is not present.
   *
   * @par Require
   * `!name.empty()`.
   */
  const std::optional<std::string>* option(const std::string& name) const
  {
    assert(!name.empty());
    const auto i = options_.find(name);
    return (i != cend(options_)) ? &(i->second) : nullptr;
  }

  /**
   * @returns `true` if the given option presents, or `false` otherwise.
   *
   * @throws `std::runtime_error` if the given option presents with an argument.
   */
  bool has_option_throw_if_argument(const std::string& name) const
  {
    if (const auto* const oa = option(name)) {
      if (!*oa)
        return true;
      else
        throw std::runtime_error{std::string{"program option --"}.append(name).append(" has no argument")};
    } else
      return false;
  }

  /// @returns Iterator to the first found option that is not presents in `list`.
  Option_map::const_iterator option_except(const std::vector<std::string>& list) const noexcept
  {
    return std::find_if(cbegin(options_), cend(options_),
      [b = cbegin(list), e = cend(list)](const auto& o)
      {
        return std::find(b, e, o.first) == e;
      });
  }

  /// @returns `true` if there are option that is not present in given `list`.
  bool has_option_except(const std::vector<std::string>& list) const noexcept
  {
    return option_except(list) != cend(options_);
  }

private:
  std::filesystem::path executable_path_;
  Option_map options_;
  Argument_vector arguments_;
};

} // namespace dmitigr::progpar

#endif // DMITIGR_MISC_PROGPAR_HPP
