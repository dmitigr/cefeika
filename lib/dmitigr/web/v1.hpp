// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or web.hpp

#ifndef DMITIGR_WEB_V1_HPP
#define DMITIGR_WEB_V1_HPP

#include "dmitigr/web/dll.hpp"
#include <dmitigr/base/filesystem.hpp>
#include <dmitigr/fcgi/fcgi.hpp>
#include <dmitigr/jrpc/jrpc.hpp>
#include <dmitigr/mulf/mulf.hpp>
#include <dmitigr/ttpl/ttpl.hpp>

#include <functional>
#include <map>
#include <string_view>

namespace dmitigr::web::v1 {

struct Handle_options final {
  using Htmler = std::function<void(fcgi::Server_connection*, ttpl::Logic_less_template&)>;
  using Caller = std::function<jrpc::Result(fcgi::Server_connection*, const jrpc::Request&)>;
  using Former = std::function<void(fcgi::Server_connection*, const mulf::Form_data&)>;
  using Custom = std::function<void(fcgi::Server_connection*)>;

  std::filesystem::path docroot;
  std::filesystem::path tplroot;
  std::string index;
  std::map<std::string_view, Htmler> htmlers;
  std::map<std::string_view, Caller> callers;
  std::map<std::string_view, Former> formers;
  std::map<std::string_view, Custom> customs;
  Custom fallback;
};

DMITIGR_WEB_API void handle(fcgi::Server_connection* fcgi, const Handle_options& opts);

} // namespace dmitigr::web::v1

#ifdef DMITIGR_WEB_HEADER_ONLY
#include "dmitigr/web/v1.cpp"
#endif

#endif // DMITIGR_WEB_V1_HPP
