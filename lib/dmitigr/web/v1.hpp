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
#include <type_traits>
#include <vector>
#include <utility>

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
    ttpl::Logic_less_template result{str::file_to_string(tplfile)};
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
  using Htmler = std::function<void(fcgi::Server_connection*, ttpl::Logic_less_template&, const std::smatch&)>;
  using Caller = std::function<jrpc::Result(fcgi::Server_connection*, const jrpc::Request&, const std::smatch&)>;
  using Former = std::function<void(fcgi::Server_connection*, const mulf::Form_data&, const std::smatch&)>;
  using Custom = std::function<void(fcgi::Server_connection*, const std::smatch&)>;
  using Fallback = std::function<void(fcgi::Server_connection*)>;

  std::filesystem::path docroot;
  std::filesystem::path tplroot;
  std::string index;
  std::vector<std::pair<std::regex, Htmler>> htmlers;
  std::vector<std::pair<std::regex, Caller>> callers;
  std::vector<std::pair<std::regex, Former>> formers;
  std::vector<std::pair<std::regex, Custom>> customs;
  Fallback fallback;
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
  DMITIGR_REQUIRE(fcgi, std::invalid_argument);

  const std::string location{fcgi->parameter("SCRIPT_NAME")->value()};
  const auto method = fcgi->parameter("REQUEST_METHOD")->value();

  static const auto match = [](const std::string& location, const auto& routing_table) ->
    std::pair<std::smatch, typename std::decay_t<decltype(routing_table)>::const_iterator>
  {
    const auto b = cbegin(routing_table);
    const auto e = cend(routing_table);
    for (auto i = b; i != e; ++i) {
      std::smatch sm;
      if (std::regex_match(location, sm, i->first)) {
        DMITIGR_REQUIRE(i->second, std::logic_error, "handler for \"" + location +"\" is not specified");
        return {std::move(sm), i};
      }
    }
    return {std::smatch{}, e};
  };

  const auto call_custom_or_fallback = [fcgi, location, &opts]
  {
    if (const auto [location_match, i] = match(location, opts.customs); location_match.ready())
      i->second(fcgi, location_match);
    else if (opts.fallback)
      opts.fallback(fcgi);
    else
      fcgi->out() << "Status: 404" << fcgi::crlfcrlf;
  };

  try {
    if (method == "GET") {
      if (const auto [location_match, i] = match(location, opts.htmlers); location_match.ready()) {
        const std::filesystem::path locpath{location};
        const std::filesystem::path tplfile = opts.docroot / locpath.relative_path() / opts.index;
        if (auto tpl = make_expanded_llt(tplfile, opts.tplroot)) {
          auto& t = *tpl;
          i->second(fcgi, t, location_match);
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
        if (const auto [location_match, i] = match(location, opts.callers); location_match.ready()) {
          std::string o;
          try {
            const jrpc::Request request{str::read_to_string(fcgi->in())};
            const auto result = i->second(fcgi, request, location_match);
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
        std::smatch boundary_match;
        if (std::regex_search(content_type, boundary_match, mpfdre)) {
          DMITIGR_ASSERT(boundary_match.size() >= 2);
          if (const auto [location_match, i] = match(location, opts.formers); location_match.ready()) {
            const auto boundary = boundary_match.str(1);
            const mulf::Form_data form{str::read_to_string(fcgi->in()), boundary};
            return i->second(fcgi, form, location_match);
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
