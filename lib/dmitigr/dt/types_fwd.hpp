// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or dt.hpp

#ifndef DMITIGR_DT_TYPES_FWD_HPP
#define DMITIGR_DT_TYPES_FWD_HPP

/**
 * @brief The API.
 */
namespace dmitigr::dt {

enum class Day_of_week;
enum class Month;

class Timestamp;

/**
 * @brief The implementation details.
 */
namespace detail {

class iTimestamp;

} // namespace detail

} // namespace dmitigr::dt

#endif  // DMITIGR_DT_TYPES_FWD_HPP
