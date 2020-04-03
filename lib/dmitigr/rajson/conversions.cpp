// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rajson.hpp

#include "dmitigr/rajson/conversions.hpp"

namespace dmitigr::rajson {

DMITIGR_RAJSON_INLINE rapidjson::Document to_parsed_json(const std::string_view input)
{
  rapidjson::Document result;
  result.Parse(input.data(), input.size());
  return result;
}

} // namespace dmitigr::rajson
