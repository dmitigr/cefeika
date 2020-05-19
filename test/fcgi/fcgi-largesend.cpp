// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#include <dmitigr/fcgi.hpp>
#include <dmitigr/rng.hpp>
#include <iostream>

int main()
{
  namespace fcgi = dmitigr::fcgi;
  namespace rng = dmitigr::rng;

  rng::seed_by_now();
  std::streamsize rndstr_size{100500};
  try {
    const auto port = 9000;
    const auto backlog = 64;
    const auto server = fcgi::Listener_options::make("0.0.0.0", port, backlog)->make_listener();
    server->listen();
    while (true) {
      if (const auto conn = server->accept()) {
        conn->out() << "Content-Type: text/plain" << fcgi::crlfcrlf;
        const std::string rndstr = rng::random_string("abcdefg", rndstr_size);
        conn->out() << rndstr << "\n";
        conn->out() << rndstr_size;
        rndstr_size += rng::cpp_pl_3rd(rndstr_size, rndstr_size + 1024);
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "Oops: " << e.what() << std::endl;
    return 1;
  }
}
