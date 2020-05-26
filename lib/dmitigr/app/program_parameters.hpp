// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or app.hpp

#ifndef DMITIGR_APP_PROGRAM_PARAMETERS_HPP
#define DMITIGR_APP_PROGRAM_PARAMETERS_HPP

#include "dmitigr/base/debug.hpp"
#include "dmitigr/base/filesystem.hpp"

#include <algorithm>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace dmitigr::app {

/**
 * @brief A program parameters.
 *
 * Stores the parsed program parameters like the following:
 *   executabe [command] [--opt1 --opt2=arg] [--] [arg1 arg2]
 *
 * Each option may have an argument which is specified after the "=" character.
 * The sequence of characters "--" indicates that the remaining parameters should
 * not be treated as options, but arguments.
 *
 * @remarks "Short" options (e.g. `-o` or `-o 1`) does not supported
 * and treated as arguments.
 */
class Program_parameters final {
public:
  /**
   * @brief The alias to represent a map of program options.
   */
  using Option_map = std::map<std::string, std::optional<std::string>>;

  /**
   * @brief The alias to represent a vector of program arguments.
   */
  using Argument_vector = std::vector<std::string>;

  /**
   * @returns The new instance.
   *
   * @par Requires
   * `(argc > 0 && argv && argv[0])`.
   */
  Program_parameters(const int argc, const char* const* argv)
  {
    DMITIGR_REQUIRE(argc > 0 && argv && argv[0], std::invalid_argument);

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

    // Extracting a command name
    if (auto cmd = opt(argv[argi]); !cmd) {
      command_name_ = std::string{argv[argi]};
      ++argi;
    }

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

    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @returns The executable path.
   */
  const std::filesystem::path& executable_path() const
  {
    return executable_path_;
  }

  /**
   * @returns The name of the command.
   */
  const std::optional<std::string>& command_name() const
  {
    return command_name_;
  }

  /**
   * @returns The map of options.
   */
  const Option_map& options() const
  {
    return options_;
  }

  /**
   * @returns The vector of arguments.
   */
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
    DMITIGR_REQUIRE(!name.empty(), std::invalid_argument);
    const auto i = options_.find(name);
    return (i != cend(options_)) ? &(i->second) : nullptr;
  }

  /**
   * @returns An option argument, or `std::nullopt` if option is not present.
   *
   * @throw std::runtime_error if argument for this option is not present.
   */
  const std::optional<std::string>& option_with_argument(const std::string& name) const
  {
    if (const auto* const o = option(name)) {
      if (*o)
        return *o;
      else
        throw std::runtime_error{"argument for option --" + name + " is not present"};
    } else
      return null_;
  }

  /**
   * @returns Iterator to the first found option that is not presents in `options`.
   */
  Option_map::const_iterator option_other_than(const std::vector<std::string>& options) const
  {
    const auto boptions = cbegin(options);
    const auto eoptions = cend(options);
    const auto e = cend(options_);
    for (auto i = cbegin(options_); i != e; ++i) {
      if (const auto ioptions = std::find(boptions, eoptions, i->first); ioptions == eoptions)
        return i;
    }
    return e;
  }

private:
  std::filesystem::path executable_path_;
  std::optional<std::string> command_name_;
  Option_map options_;
  Argument_vector arguments_;
  static const std::optional<std::string> null_;

  bool is_invariant_ok() const
  {
    return !executable_path_.empty();
  }

};

} // namespace dmitigr::app

#endif // DMITIGR_APP_PROGRAM_PARAMETERS_HPP
