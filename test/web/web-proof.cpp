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

  web::Handle_options ho = {
    // docroot
    "/",
    // tplroot
    "/",
    // index
    "index.html",
    // htmlers
    {{"/", [](fcgi::Server_connection*, ttpl::Logic_less_template*) {}}},
    // callers
    {{"/api", [](fcgi::Server_connection*, const jrpc::Request&) { return jrpc::Result{}; }}},
    // formers
    {{"/", [](fcgi::Server_connection*, const mulf::Form_data*){}}},
    // customs
    {{"/", [](fcgi::Server_connection*){}}},
    // fallback
    [](fcgi::Server_connection*){}
  };
  web::handle(nullptr, ho);
}
