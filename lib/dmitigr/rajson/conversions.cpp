// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rajson.hpp

#include "dmitigr/rajson/conversions.hpp"
#include "dmitigr/rajson/implementation_header.hpp"

DMITIGR_RAJSON_INLINE rapidjson::Document to_parsed_json(const std::string_view input)
{
  rapidjson::Document result;
  result.Parse(input.data(), input.size());
  if (result.HasParseError())
    throw std::runtime_error{"dmitigr::json::to_parsed_json(): parse error"};
  return result;
}

#include "dmitigr/rajson/implementation_footer.hpp"
