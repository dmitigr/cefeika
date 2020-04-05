// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or web.hpp

#include "dmitigr/web/v1.hpp"
#include <dmitigr/base/debug.hpp>
#include <dmitigr/http/http.hpp>
#include <dmitigr/str/str.hpp>

#include <regex>

namespace dmitigr::web::v1 {

namespace {

DMITIGR_WEB_INLINE std::unique_ptr<ttpl::Logic_less_template>
make_ttpl_deep(const std::filesystem::path& path, const std::filesystem::path& tplroot)
{
  if (std::filesystem::exists(path)) {
    auto result = ttpl::Logic_less_template::make(str::file_to_string(path));
    for (std::size_t i = 0, pcount = result->parameter_count(); i < pcount;) {
      const auto& pname = result->parameter(i)->name();
      if (auto t = make_ttpl_deep(tplroot / pname, tplroot)) {
        result->replace_parameter(pname, t.get());
        pcount = result->parameter_count(); // parameters might change after replace
      } else
        ++i;
    }
    return result;
  } else
    return nullptr;
}

} // namespace

DMITIGR_WEB_INLINE void handle(fcgi::Server_connection* const fcgi, const Handle_options& opts)
{
  DMITIGR_REQUIRE(fcgi, std::invalid_argument);

  const auto location = fcgi->parameter("SCRIPT_NAME")->value();
  const auto method = fcgi->parameter("REQUEST_METHOD")->value();

  const auto call_custom_or_fallback = [fcgi, location, &opts]
  {
    if (const auto i = opts.customs.find(location); i != opts.customs.cend()) {
      DMITIGR_REQUIRE(i->second, std::logic_error,
        "custom handler for \"" + std::string{location} +"\" is unset");
      i->second(fcgi);
    } else if (opts.fallback)
      opts.fallback(fcgi);
    else
      fcgi->out() << "Status: 404" << fcgi::crlfcrlf;
  };

  try {
    if (method == "GET") {
      if (const auto i = opts.htmlers.find(location); i != opts.htmlers.cend()) {
        const std::filesystem::path locpath{location};
        const std::filesystem::path tplfile = opts.docroot / locpath.relative_path() / opts.index;
        if (const auto tpl = make_ttpl_deep(tplfile, opts.tplroot)) {
          DMITIGR_REQUIRE(i->second, std::logic_error,
            "htmler handler for \"" + std::string{location} +"\" is unset");
          i->second(fcgi, tpl.get());
          const auto o = tpl->to_output();
          fcgi->out() << "Content-Type: text/html" << fcgi::crlfcrlf;
          fcgi->out().write(o.data(), o.size());
        } else {
          fcgi->out() << "Status: 404" << fcgi::crlfcrlf;
        }
        return;
      }
    } else if (method == "POST") {
      const std::string content_type{fcgi->parameter("CONTENT_TYPE")->value()};
      if (content_type == "application/json") {
        if (const auto i = opts.callers.find(location); i != opts.callers.cend()) {
          DMITIGR_REQUIRE(i->second, std::logic_error,
            "caller handler for \"" + std::string{location} +"\" is unset");
          std::string o;
          try {
            const auto request = jrpc::Request::make(str::read_to_string(fcgi->in()));
            const auto response = i->second(fcgi, request.get());
            o = response->to_string();
          } catch (const jrpc::Error& e) {
            o = e.to_string();
          }
          fcgi->out() << "Content-Type: application/json" << fcgi::crlfcrlf;
          fcgi->out().write(o.data(), o.size());
          return;
        }
      } else {
        // bcharnospace := -'()+,./:=?\w
        // boundary=[bcharsnospace or space]{1,69}[bcharsnospace]{1}
        static const std::regex mpfdre{
          R"(multipart/form-data;\s*boundary="?([-'()+,./:=?\w\s]{1,69}[-'()+,./:=?\w]{1}))" "\"?",
          std::regex::icase | std::regex::optimize};
        std::smatch sm;
        if (std::regex_search(content_type, sm, mpfdre)) {
          DMITIGR_ASSERT(sm.size() >= 2);
          if (const auto i = opts.formers.find(location); i != opts.formers.cend()) {
            DMITIGR_REQUIRE(i->second, std::logic_error,
              "former handler for \"" + std::string{location} +"\" is unset");
            const auto boundary = sm.str(1);
            const auto form = mulf::Form_data::make(str::read_to_string(fcgi->in()), boundary);
            return i->second(fcgi, form.get());
          }
        }
      }
    }
  } catch (const http::Server_exception& e) {
    DMITIGR_REQUIRE(!fcgi->out().tellp(), std::logic_error,
      "http::Server_exception thrown but some data is already sent");
    fcgi->out() << "Status: " << e.code().value() << fcgi::crlfcrlf;
    return;
  }

  call_custom_or_fallback();
}

} // namespace dmitigr::web::v1
