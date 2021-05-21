// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or rajson.hpp

#ifndef DMITIGR_RAJSON_FWD_HPP
#define DMITIGR_RAJSON_FWD_HPP

#include <cstddef>
#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif
#ifndef RAPIDJSON_NO_SIZETYPEDEFINE
#define RAPIDJSON_NO_SIZETYPEDEFINE
#endif
namespace rapidjson {
using SizeType = std::size_t;
} // namespace rapidjson

#include "../3rdparty/rapidjson/fwd.h"

#endif  // DMITIGR_RAJSON_FWD_HPP
