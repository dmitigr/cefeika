// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or app.hpp

#include <dmitigr/app.hpp>
#include <dmitigr/testo.hpp>

#include <iostream>

namespace proc = dmitigr::app::proc1;

void start()
{
  proc::set_cleanup();
  std::clog << "The application is started!" << std::endl;
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
