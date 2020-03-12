// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or web.hpp

#include "dmitigr/web/web.hpp"
#include "dmitigr/web/implementation_header.hpp"

#include "dmitigr/str.hpp"
#include "dmitigr/util/debug.hpp"

#include <regex>

namespace dmitigr::web {

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
      DMITIGR_ASSERT_ALWAYS(i->second);
      return i->second(fcgi);
    } else if (opts.fallback)
      return opts.fallback(fcgi);

    fcgi->out() << "Status: 404" << fcgi::crlfcrlf;
  };

  if (method == "GET") {
    const std::filesystem::path locpath{location};
    const std::filesystem::path tplfile = opts.docroot / locpath.relative_path() / opts.index;
    if (const auto tpl = make_ttpl_deep(tplfile, opts.tplroot)) {
      if (const auto i = opts.htmlers.find(location); i != opts.htmlers.cend()) {
        DMITIGR_REQUIRE(i->second, std::logic_error,
          "htmler for \"" + std::string{location} + "\" is unset");
        // TODO: catch HTTP error code and set the Status header.
        i->second(fcgi, tpl.get());
        const auto o = tpl->to_output();
        fcgi->out() << "Content-Type: text/html" << fcgi::crlfcrlf;
        fcgi->out().write(o.data(), o.size());
      } else {
        fcgi->out() << "Status: 500" << fcgi::crlfcrlf;
      }
      return;
    }
  } else if (method == "POST") {
    const std::string content_type{fcgi->parameter("CONTENT_TYPE")->value()};
    if (content_type == "application/json") {
      if (const auto i = opts.callers.find(location); i != opts.callers.cend()) {
        DMITIGR_REQUIRE(i->second, std::logic_error,
          "caller for \"" + std::string{location} + "\" is unset");
        try {
          const auto request = jrpc::Request::make(str::read_to_string(fcgi->in()));
          const auto response = i->second(fcgi, request.get());
          const auto o = response->to_string();
          fcgi->out() << "Content-Type: application/json" << fcgi::crlfcrlf;
          fcgi->out().write(o.data(), o.size());
        } catch (const jrpc::Error& e) {
          const auto o = e.to_string();
          fcgi->out() << "Content-Type: application/json" << fcgi::crlfcrlf;
          fcgi->out().write(o.data(), o.size());
        }
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
        if (const auto i = opts.formers.find(location); i != opts.formers.cend()) {
          DMITIGR_REQUIRE(i->second, std::logic_error,
            "former for \"" + std::string{location} +"\" is unset");
          const auto boundary = sm.str(1);
          const auto form = mulf::Form_data::make(str::read_to_string(fcgi->in()), boundary);
          return i->second(fcgi, form.get());
        }
      }
    }
  }

  call_custom_or_fallback();
}

} // namespace dmitigr::web

#include "dmitigr/web/implementation_footer.hpp"
