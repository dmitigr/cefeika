// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or web.hpp

#include "dmitigr/web/web.hpp"
#include "dmitigr/web/implementation_header.hpp"

#include "dmitigr/util/debug.hpp"

namespace dmitigr::web {

DMITIGR_WEB_INLINE void handle(fcgi::Server_connection* const fcgi, const Handle_options& opts)
{
  DMITIGR_REQUIRE(fcgi, std::invalid_argument);

  const auto location = fcgi->parameter("SCRIPT_NAME")->value();
  const auto method = fcgi->parameter("REQUEST_METHOD")->value();

  const auto call_custom_or_fallback = [fcgi, location, &opts]
  {
    if (const auto i = opts.customs.find(location); i != opts.customs.cend()) {
      DMITIGR_ASSERT_ALWAYS(i->second);
      return i->second(fcgi);
    } else if (opts.fallback)
      return opts.fallback(fcgi);

    fcgi->out() << "Status: 404" << fcgi::crlfcrlf;
  };

  if (method == "GET") {
    // check if the template exists
    const std::filesystem::path template_file;
    if (!template_file.empty()) {
      if (const auto i = opts.loaders.find(location); i != opts.loaders.cend()) {
        DMITIGR_ASSERT_ALWAYS(i->second);
        // create template object ...
        return i->second(fcgi, nullptr);
      }
    }
  } else if (method == "POST") {
    const auto content_type = fcgi->parameter("CONTENT_TYPE")->value();
    if (content_type == "application/json") {
      if (const auto i = opts.callers.find(location); i != opts.callers.cend()) {
        DMITIGR_ASSERT_ALWAYS(i->second);
        // create JSON-RPC Request...
        return i->second(fcgi, nullptr);
      }
    } else if (content_type == "multipart/form-data") {
      if (const auto i = opts.formers.find(location); i != opts.formers.cend()) {
        DMITIGR_ASSERT_ALWAYS(i->second);
        // create Form_data instance...
        return i->second(fcgi, nullptr);
      }
    }
  }

  call_custom_or_fallback();
}

} // namespace dmitigr::web

#include "dmitigr/web/implementation_footer.hpp"
