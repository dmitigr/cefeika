// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or util.hpp

#include <dmitigr/util.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
  namespace app = dmitigr::app;
  using namespace dmitigr::test;

  try {
    auto po = app::Program_parameters::make(argc, argv);
    const auto epath = po->executable_path();
    ASSERT(!epath.empty());
    std::cout << "Executable path: " << epath << std::endl;

    const auto& cname = po->command_name();
    std::cout << "Command name: " << cname.value_or("<UNSPECIFIED>") << std::endl;

    const auto& opts = po->options();
    std::cout << opts.size() << " options specified";
    if (!opts.empty()) {
      std::cout << ":" << std::endl;
      for (const auto& o : opts) {
        std::cout << "  " << o.first;
        if (o.second)
          std::cout << " = " << *o.second;
        std::cout << std::endl;
      }
    } else
      std::cout << "." << std::endl;

    const auto& args = po->arguments();
    std::cout << args.size() << " arguments specified";
    if (!args.empty()) {
      std::cout << ":" << std::endl;
      for (const auto& a : args)
        std::cout << "  " << a << std::endl;
    } else
      std::cout << "." << std::endl;
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 2;
  }
  return 0;
}
