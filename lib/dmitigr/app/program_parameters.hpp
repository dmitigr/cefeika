// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or app.hpp

#ifndef DMITIGR_APP_PROGRAM_PARAMETERS_HPP
#define DMITIGR_APP_PROGRAM_PARAMETERS_HPP

#include "dmitigr/app/dll.hpp"
#include "dmitigr/app/types_fwd.hpp"
#include "dmitigr/fs.hpp"

#include <map>
#include <memory>
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
class Program_parameters {
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
   * @brief The destructor.
   */
  virtual ~Program_parameters() = default;

  /// @name Constructors
  /// @{

  /**
   * @returns The new instance.
   *
   * @par Requires
   * `(argc > 0 && argv && argv[0])`.
   */
  static DMITIGR_APP_API std::unique_ptr<Program_parameters> make(int argc,
    const char* const* argv);

  /// @}

  /**
   * @returns The executable path.
   */
  virtual const std::filesystem::path& executable_path() const = 0;

  /**
   * @returns The name of the command.
   */
  virtual const std::optional<std::string>& command_name() const = 0;

  /**
   * @returns The map of options.
   */
  virtual const Option_map& options() const = 0;

  /**
   * @returns The vector of arguments.
   */
  virtual const Argument_vector& arguments() const = 0;

  /**
   * @returns `true` if option with `name` is presents, or `false` otherwise.
   *
   * @par Require
   * `!name.empty()`.
   */
  virtual bool has_option(const std::string& name) = 0;

private:
  friend detail::iProgram_parameters;

  Program_parameters() = default;
};

} // namespace dmitigr::app

#ifdef DMITIGR_APP_HEADER_ONLY
#include "dmitigr/app/program_parameters.cpp"
#endif

#endif // DMITIGR_APP_PROGRAM_PARAMETERS_HPP
