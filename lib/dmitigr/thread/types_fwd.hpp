// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or thread.hpp

#ifndef DMITIGR_THREAD_TYPES_FWD_HPP
#define DMITIGR_THREAD_TYPES_FWD_HPP

namespace dmitigr {

/**
 * @brief The API.
 */
namespace thread {

class Simple_threadpool;

/**
 * @brief The implementation detail.
 */
namespace detail {
class iSimple_threadpool;
} // namespace detail
} // namespace thread
} // namespace dmitigr

#endif  // DMITIGR_THREAD_TYPES_FWD_HPP
