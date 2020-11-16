// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or web.hpp

#ifndef DMITIGR_WEB_V1_HPP
#define DMITIGR_WEB_V1_HPP

#include <dmitigr/fcgi/fcgi.hpp>
#include <dmitigr/http/http.hpp>
#include <dmitigr/jrpc/jrpc.hpp>
#include <dmitigr/misc/filesystem.hpp>
#include <dmitigr/misc/mulf.hpp>
#include <dmitigr/misc/read.hpp>
#include <dmitigr/misc/str.hpp>
#include <dmitigr/misc/ttpl.hpp>

#include <cassert>
#include <functional>
#include <map>
#include <optional>
#include <regex>
#include <string_view>
#include <vector>

namespace dmitigr::web::v1 {

namespace detail {

/// @see make_expanded_llt(const std::filesystem::path&, const std::filesystem::path&).
inline std::optional<ttpl::Logic_less_template>
make_expanded_llt(const std::filesystem::path& tplfile, const std::filesystem::path& tplroot,
  std::vector<std::filesystem::path>& referenced)
{
  const auto e = cend(referenced);
  if (std::find(cbegin(referenced), e, tplfile) != e) {
    auto graph = str::to_string(referenced, " -> ", [](const auto& e) { return e.string(); });
    graph.append(" -> ").append(tplfile.string());
    throw std::logic_error{"reference cyclicity detected: " + graph};
  } else
    referenced.push_back(tplfile);

  if (std::filesystem::exists(tplfile)) {
    ttpl::Logic_less_template result{read::file_to_string(tplfile)};
    for (std::size_t i = 0, pcount = result.parameter_count(); i < pcount;) {
      const auto& pname = result.parameter(i).name();
      if (auto t = make_expanded_llt(tplroot / pname, tplroot, referenced)) {
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
  Custom fallback;
};

/**
 * @brief Creates the logic less template recursively.
 *
 * @param tplfile Path to the text template file
 * @param tplroot Path to the directory with text template files which can be
 * referenced (included) from `tplfile` and/or from each others.
 */
inline std::optional<ttpl::Logic_less_template>
make_expanded_llt(const std::filesystem::path& tplfile, const std::filesystem::path& tplroot)
{
  std::vector<std::filesystem::path> referenced;
  return detail::make_expanded_llt(tplfile, tplroot, referenced);
}

/**
 * @brief Handles the accepted FastCGI connection.
 *
 * @param fcgi Connection to handle
 * @param opts Options for handling
 */
inline void handle(fcgi::Server_connection* const fcgi, const Handle_options& opts)
{
  assert(fcgi);

  const auto location = fcgi->parameter("SCRIPT_NAME")->value();
  const auto method = fcgi->parameter("REQUEST_METHOD")->value();

  const auto hins = [location](std::string htype)
  {
    return htype.append(" for \"").append(location).append("\" is not specified");
  };

  try {
    if (method == "GET") {
      if (const auto i = opts.htmlers.find(location); i != opts.htmlers.cend()) {
        const std::filesystem::path locpath{location};
        const std::filesystem::path tplfile = opts.docroot / locpath.relative_path() / opts.index;
        if (auto tpl = make_expanded_llt(tplfile, opts.tplroot)) {
          if (!i->second)
            throw std::logic_error{hins("htmler")};
          auto& t = *tpl;
          i->second(fcgi, t);
          const auto o = t.to_output();
          fcgi->out() << "Content-Type: text/html" << fcgi::crlfcrlf;
          fcgi->out().write(o.data(), o.size());
        } else
          fcgi->out() << "Status: 404" << fcgi::crlfcrlf;
        return;
      }
    } else if (method == "POST") {
      const std::string content_type{fcgi->parameter("CONTENT_TYPE")->value()};
      if (content_type == "application/json") {
        if (const auto i = opts.callers.find(location); i != opts.callers.cend()) {
          if (!i->second)
            throw std::logic_error{hins("caller")};
          std::string o;
          try {
            const jrpc::Request request{read::to_string(fcgi->in())};
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
          assert(sm.size() >= 2);
          if (const auto i = opts.formers.find(location); i != opts.formers.cend()) {
            if (!i->second)
              throw std::logic_error{hins("former")};
            const auto boundary = sm.str(1);
            const mulf::Form_data form{read::to_string(fcgi->in()), boundary};
            return i->second(fcgi, form);
          }
        }
      }
    }

    if (opts.fallback)
      opts.fallback(fcgi);
    else
      fcgi->out() << "Status: 404" << fcgi::crlfcrlf;
  } catch (const http::Server_exception& e) {
    if (!fcgi->out().tellp())
      fcgi->out() << "Status: " << e.code().value() << fcgi::crlfcrlf;
    else
      throw std::logic_error{"http::Server_exception thrown but some data is already sent"};
  }
}

} // namespace dmitigr::web::v1

#endif // DMITIGR_WEB_V1_HPP
