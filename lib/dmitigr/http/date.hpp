// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#ifndef DMITIGR_HTTP_DATE_HPP
#define DMITIGR_HTTP_DATE_HPP

#include "dmitigr/http/dll.hpp"
#include "dmitigr/http/header.hpp"
#include "dmitigr/http/types_fwd.hpp"
#include <dmitigr/dt/timestamp.hpp>

#include <memory>
#include <string>

namespace dmitigr::http {

/**
 * @ingroup headers
 *
 * @brief A HTTP Date header.
 */
class Date : public Header {
public:
  /**
   * @brief The destructor.
   */
  virtual ~Date() = default;

  /// @name Constructors
  /// @{

  /**
   * @brief Constructs the object by parsing the `input`.
   *
   * Examples of valid input are:
   *
   *   1. Sat, 06 Apr 2019 17:00:00 GMT
   *
   * @param input - the http-date.
   *
   * @see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Date
   */
  static DMITIGR_HTTP_API std::unique_ptr<Date> make(std::string_view input);

  /**
   * @overload
   *
   * @par Requires
   * `(ts != nullptr)`.
   */
  static DMITIGR_HTTP_API std::unique_ptr<Date> make(const dt::Timestamp& ts);

  /**
   * @returns The copy of this instance.
   */
  virtual std::unique_ptr<Date> to_date() const = 0;

  /// @}

  /**
   * @returns The timestamp.
   */
  virtual const dt::Timestamp& timestamp() const = 0;

  /**
   * @overload
   */
  virtual dt::Timestamp& timestamp() = 0;

  /**
   * @brief Sets the timestamp.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(ts != nullptr)`.
   */
  virtual void set_timestamp(dt::Timestamp ts) = 0;

private:
  friend detail::iDate;

  Date() = default;
};

} // namespace dmitigr::http

#ifdef DMITIGR_HTTP_HEADER_ONLY
#include "dmitigr/http/date.cpp"
#endif

#endif  // DMITIGR_HTTP_DATE_HPP
