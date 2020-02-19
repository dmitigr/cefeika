// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or process.hpp

#include <dmitigr/process.hpp>
#include <dmitigr/util/test.hpp>

#include <csignal>
#include <iostream>

inline std::filesystem::path pid_file;
inline std::filesystem::path log_file;

int main(int, char* argv[])
{
  namespace proc = dmitigr::process;
  using namespace dmitigr::test;

  try {
    const auto dirname = std::filesystem::path{argv[0]}.parent_path();
    pid_file = std::filesystem::absolute(dirname/"process-detach.pid");
    log_file = std::filesystem::absolute(dirname/"process-detach.log");
    proc::detach([]
    {
      const auto cleanup = []
      {
        std::clog << "Cleaning up..." << std::endl;
        {
          std::clog << "Attempting to remove the PID file " << pid_file << " ... ";
          std::error_code e;
          if (std::filesystem::remove(pid_file, e))
            std::clog << "Ok" << std::endl;
          else
            std::clog << "Failed" << std::endl;
        }
      };

      std::set_terminate(cleanup);
      std::at_quick_exit(cleanup);
      std::atexit(cleanup);

      std::signal(SIGHUP,  &std::quick_exit);
      std::signal(SIGINT,  &std::quick_exit);
      std::signal(SIGTERM, &std::quick_exit);

      std::clog << "Detached process done." << std::endl;
    }, pid_file, log_file);
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 2;
  }
  return 0;
}
