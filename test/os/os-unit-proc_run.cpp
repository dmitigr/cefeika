// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt

#include <dmitigr/misc/testo.hpp>
#include <dmitigr/os/proc_run.hpp>

#include <iostream>

namespace proc = dmitigr::os::proc;

void start()
{
  std::clog << "The process is started!" << std::endl;
  std::clog << "Start flag is " << proc::is_running << std::endl;
}

int main(int argc, char* argv[])
{
  using namespace dmitigr::testo;
  proc::prog_params = {argc, argv};
  const std::string info = "[--detach]";
  if (proc::prog_params.has_option_except({"detach"}) || proc::prog_params.arguments().size() > 0)
    proc::usage(info);
  const bool detach = proc::prog_params.has_option_throw_if_argument("detach");
  proc::start(detach, start);
}
