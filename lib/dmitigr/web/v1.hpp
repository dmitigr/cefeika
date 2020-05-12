// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or web.hpp

#ifndef DMITIGR_WEB_V1_HPP
#define DMITIGR_WEB_V1_HPP

#include <dmitigr/base/debug.hpp>
#include <dmitigr/base/filesystem.hpp>
#include <dmitigr/fcgi/fcgi.hpp>
#include <dmitigr/http/http.hpp>
#include <dmitigr/jrpc/jrpc.hpp>
#include <dmitigr/mulf/mulf.hpp>
#include <dmitigr/ttpl/ttpl.hpp>
#include <dmitigr/str/str.hpp>

#include <functional>
#include <map>
#include <optional>
#include <regex>
#include <string_view>

namespace dmitigr::web::v1 {

namespace detail {

/**
 * @brief Creates the logic less template recursively.
 *
 * @param tplfile Path to the text template file
 * @param tplroot Path to the directory with text template files which can be
 * referenced from `tplfile` and/or from each others.
 */
inline std::optional<ttpl::Logic_less_template>
make_llt_deep(const std::filesystem::path& tplfile, const std::filesystem::path& tplroot)
{
  if (std::filesystem::exists(tplfile)) {
    ttpl::Logic_less_template result{str::file_to_string(tplfile)};
    for (std::size_t i = 0, pcount = result.parameter_count(); i < pcount;) {
      const auto& pname = result.parameter(i).name();
      if (auto t = make_llt_deep(tplroot / pname, tplroot)) {
        result.replace_parameter(pname, *t);
        pcount = result.parameter_count(); // parameters might change after replace
      } else
        ++i;
    }
    return result;
  } else
    return std::nullopt;
}

} // namespace detail

/**
 * @brief Represents options to be passed to
 * handle(fcgi::Server_connection*, const Handle_options&).
 */
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

/**
 * @brief Handles the accepted FastCGI connection.
 *
 * @param fcgi Connection to handle
 * @param opts Options for handling
 */
inline void handle(fcgi::Server_connection* const fcgi, const Handle_options& opts)
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
        if (auto tpl = detail::make_llt_deep(tplfile, opts.tplroot)) {
          DMITIGR_REQUIRE(i->second, std::logic_error,
            "htmler handler for \"" + std::string{location} +"\" is unset");
          auto& t = *tpl;
          i->second(fcgi, t);
          const auto o = t.to_output();
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
            const jrpc::Request request{str::read_to_string(fcgi->in())};
            const auto result = i->second(fcgi, request);
            o = result.to_string();
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
            const mulf::Form_data form{str::read_to_string(fcgi->in()), boundary};
            return i->second(fcgi, form);
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

#endif // DMITIGR_WEB_V1_HPP
