// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or service.hpp

#ifndef DMITIGR_APP_PROC1_HPP
#define DMITIGR_APP_PROC1_HPP

#include "dmitigr/app/program_parameters.hpp"
#include "dmitigr/base/debug.hpp"
#include "dmitigr/base/filesystem.hpp"
#include "dmitigr/os/log.hpp"
#include "dmitigr/os/proc_detach.hpp"

#include <atomic>
#include <csignal>
#include <cstdlib>
#include <exception> // set_terminate()
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string_view>

namespace dmitigr::app::proc1 {

/// Stores a current running status of the program.
inline std::atomic_bool is_running;

/// Stores a path to a PID file.
inline std::filesystem::path pid_file;

/// Stores a path to a log file.
inline std::filesystem::path log_file;

/// Stores the program parameters. (Should be set in main()!)
inline Program_parameters prog_params;

// =============================================================================

/**
 * @brief Prints the usage info on the standard error and terminates the
 * program with unsuccessful exit code.
 *
 * @par Requires
 * `prog_params.is_valid()`.
 *
 * @param info A formatted information to print.
 */
[[noreturn]] inline void usage(const std::string_view info = {})
{
  DMITIGR_REQUIRE(proc1::prog_params.is_valid(), std::logic_error);

  std::cerr << "usage: " << proc1::prog_params.executable_path();
  if (!info.empty())
    std::cerr << " " << info;
  std::cerr << std::endl;
  std::exit(EXIT_FAILURE);
}

// =============================================================================

/**
 * @brief A typical signal handler.
 */
inline void default_handle_signal(const int sig) noexcept
{
  if (sig == SIGINT)
    proc1::is_running = false; // should cause a normal shutdown
  else if (sig == SIGTERM)
    std::quick_exit(sig); // abnormal shutdown
}

/**
 * @brief Removes a file associated with proc1::pid_file and clears
 * `pid_file` global variable on success.
 */
inline void default_cleanup() noexcept
{
  if (const bool do_cleanup = !proc1::pid_file.empty(); do_cleanup) {
    if (!proc1::pid_file.empty()) {
      std::error_code e;
      std::filesystem::remove(proc1::pid_file, e);
      if (!e)
        proc1::pid_file.clear();
      else
        std::clog << "Cannot remove PID file: " << e.value() << std::endl;
    }
  }
}

/**
 * @brief Assigns the `cleanup` as a handler of:
 *   - std::set_terminate();
 *   - std::at_quick_exit();
 *   - std::atexit().
 */
inline void set_cleanup(void(*cleanup)() = &default_cleanup) noexcept
{
  std::set_terminate(cleanup);
  std::at_quick_exit(cleanup);
  std::atexit(cleanup);
}

// =============================================================================

/**
 * @brief Calls `startup` in the current process.
 *
 * @param detach Denotes should the process be forked or not.
 * @param startup A function to call. This function is called in a current
 * process if `!detach`, or in a forked process otherwise
 * @param working_directory A path to a new working directory. If not specified
 * the directory of executable is assumed.
 * @param pid_file A path to a PID file. If not specified the name of executable
 * with ".pid" extension in the working directory is assumed.
 * @param log_file A path to a log file. If not specified the name of executable
 * with ".log" extension in the working directory is assumed.
 * @param log_file_mode A file mode for the log file.
 *
 * @par Requires
 * `prog_params.is_valid()`.
 */
inline void start(const bool detach,
  const std::function<void()>& startup,
  std::filesystem::path working_directory = {},
  std::filesystem::path pid_file = {},
  std::filesystem::path log_file = {},
  const std::ios_base::openmode log_file_mode = std::ios_base::trunc | std::ios_base::out)
{
  DMITIGR_REQUIRE(proc1::prog_params.is_valid(), std::logic_error);

  // Preparing.

  if (working_directory.empty())
    working_directory = proc1::prog_params.executable_path().parent_path();

  if (detach) {
    if (pid_file.empty()) {
      pid_file = working_directory / proc1::prog_params.executable_path().filename();
      pid_file += ".pid";
    }
    if (log_file.empty()) {
      log_file = working_directory / proc1::prog_params.executable_path().filename();
      log_file += ".log";
    }
  }

  proc1::pid_file = std::move(pid_file);
  proc1::log_file = std::move(log_file);

  // Starting.

  if (!detach) {
    if (!working_directory.empty())
      std::filesystem::current_path(working_directory);

    if (!proc1::pid_file.empty())
      os::proc::dump_pid(proc1::pid_file);

    if (!proc1::log_file.empty())
      os::proc::redirect_clog(proc1::log_file, log_file_mode);

    startup();
  } else
    os::proc::detach(startup, working_directory, proc1::pid_file, proc1::log_file, log_file_mode);
}

// =============================================================================

/**
 * @brief Calls the function `f`.
 *
 * If the call of `callback` fails with exception `proc1::is_running` flag is
 * sets to `false` which should cause the normal application shutdown.
 *
 * @param f A function to call
 * @param where A descriptive context of call for printing to `std::clog`.
 */
template<typename F>
auto with_shutdown_on_error(F&& f, const std::string_view where) noexcept
{
  try {
    return f();
  } catch (const std::exception& e) {
    std::clog << where << ": " << e.what() << ". Shutting down!\n";
    proc1::is_running = false; // normal shutdown
  } catch (...) {
    std::clog << where << ": unknown error! Shutting down!\n";
    proc1::is_running = false; // normal shutdown
  }
}

} // namespace dmitigr::app::proc1

#endif  // DMITIGR_APP_PROC1_HPP
