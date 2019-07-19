// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_BASICS_HPP
#define DMITIGR_HTTP_BASICS_HPP

#include "dmitigr/http/dll.hpp"

#include <string>

namespace dmitigr::http {

/**
 * @ingroup headers
 *
 * @brief A value of "SameSite" cookie attribute.
 */
enum class Same_site { strict, lax };

/**
 * @ingroup headers
 *
 * @returns The result of conversion of `str` to the value of type Same_site.
 *
 * @remarks The value of `str` is case-sensitive.
 */
DMITIGR_HTTP_API Same_site to_same_site(std::string_view str);

/**
 * @ingroup headers
 *
 * @returns The result of conversion of `ss` to the instance of type `std::string`.
 */
DMITIGR_HTTP_API std::string to_string(Same_site ss);

} // namespace dmitigr::http

#ifdef DMITIGR_HTTP_HEADER_ONLY
#include "dmitigr/http/basics.cpp"
#endif

#endif  // DMITIGR_HTTP_BASICS_HPP
