// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or process.hpp

#ifndef _WIN32 // Currently, not usable on Windows.

#ifndef DMITIGR_PROCESS_DETACH_HPP
#define DMITIGR_PROCESS_DETACH_HPP

#include "dmitigr/process/dll.hpp"
#include "dmitigr/process/info.hpp"
#include "dmitigr/util/filesystem.hpp"

#include <functional>

namespace dmitigr::process {

/**
 * @brief Detaches the process to make it work in background.
 *
 * @param log_file The log file the detached process will use as the
 * destination instead of `std::clog` to write the log info.
 *
 * @throws `std::runtime_error` on failure.
 *
 * @remarks The function returns in the detached (forked) process!
 */
DMITIGR_PROCESS_API void detach(std::function<void()> start,
  const std::filesystem::path& pid_file, const std::filesystem::path& log_file);

} // namespace dmitigr::process

#ifdef DMITIGR_PROCESS_HEADER_ONLY
#include "dmitigr/process/detach.cpp"
#endif

#endif  // DMITIGR_PROCESS_DETACH_HPP

#endif  // _WIN32
