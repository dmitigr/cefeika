// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_HEADER_HPP
#define DMITIGR_HTTP_HEADER_HPP

#include "dmitigr/http/types_fwd.hpp"

#include <memory>
#include <string>

namespace dmitigr::http {

/**
 * @ingroup headers
 *
 * @brief A HTTP header.
 */
class Header {
public:
  /**
   * @brief The destructor.
   */
  virtual ~Header() = default;

  /// @name Constructors
  /// @{

  /**
   * @returns The copy of this instance.
   */
  virtual std::unique_ptr<Header> to_header() const = 0;

  /// @}

  /**
   * @returns The field name of the header in a HTTP message.
   */
  virtual const std::string& field_name() const = 0;

  /// @name Conversions
  /// @{

  /**
   * @returns The result of conversion of this instance to the instance of type `std::string`.
   */
  virtual std::string to_string() const = 0;

  /// @}

private:
  friend Cookie;
  friend Date;
  friend Set_cookie;

  Header() = default;
};

} // namespace dmitigr::http

#endif  // DMITIGR_HTTP_HEADER_HPP
