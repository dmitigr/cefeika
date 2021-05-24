// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt

#include "../../testo.hpp"
#include "../../os/proc_run.hpp"

#include <iostream>

namespace proc = dmitigr::os::proc;

void start()
{
  std::clog << "The process is started!" << std::endl;
  std::clog << "Start flag is " << proc::is_running << std::endl;
}

int main(int argc, char* argv[]) try {
  using namespace dmitigr::testo;
  using proc::prog_params;
  prog_params = {argc, argv};
  const std::string info{"[--detach]"};
  const auto [detach_o] = prog_params.options("detach");
  if (prog_params.options().size() > 1 || !prog_params.arguments().empty())
    proc::usage(info);

  const bool detach = detach_o.throw_if_value();
  proc::start(detach, start);
 } catch (const std::exception& e) {
  std::clog << argv[0] << ": " << e.what() << std::endl;
 }
