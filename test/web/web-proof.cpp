// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt

#include "dmitigr/web.hpp"

int main()
{
  namespace web = dmitigr::web::v1;
  namespace fcgi = dmitigr::fcgi;
  namespace ttpl = dmitigr::ttpl;
  namespace jrpc = dmitigr::jrpc;
  namespace mulf = dmitigr::mulf;
  using re = std::regex;
  web::Handle_options ho = {
    // docroot
    "/",
    // tplroot
    "/",
    // index
    "index.html",
    // htmlers
    {{re{"/"}, [](fcgi::Server_connection*, ttpl::Logic_less_template&, const std::smatch&) {}}},
    // callers
    {{re{"/api"}, [](fcgi::Server_connection*, const jrpc::Request&, const std::smatch&) { return jrpc::Result{}; }}},
    // formers
    {{re{"/"}, [](fcgi::Server_connection*, const mulf::Form_data&, const std::smatch&){}}},
    // customs
    {{re{"/"}, [](fcgi::Server_connection*, const std::smatch&){}}},
    // fallback
    [](fcgi::Server_connection*){}
  };
  web::handle(nullptr, ho);
}
