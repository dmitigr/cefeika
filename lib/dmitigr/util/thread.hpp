// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or util.hpp

#ifndef DMITIGR_UTIL_THREAD_HPP
#define DMITIGR_UTIL_THREAD_HPP

#include "dmitigr/util/types_fwd.hpp"
#include "dmitigr/util/dll.hpp"

#include <cstddef>
#include <functional>
#include <memory>

namespace dmitigr::thread {

/**
 * @brief Simple threadpool.
 */
class Simple_threadpool {
public:
  /**
   * @brief The destructor.
   */
  virtual ~Simple_threadpool() = default;

  /// @name Constructors
  /// @{

  /**
   * @brief Constructs the threadpool with size of `size`.
   *
   * @par Requires
   * `(size > 0 && queue_max_size > 0)`.
   */
  static DMITIGR_UTIL_API std::unique_ptr<Simple_threadpool> make(std::size_t size);

  /// @}

  /**
   * @brief Submit the function to run on the threadpool.
   *
   * @par Requires
   * `(function)`.
   */
  virtual void submit(std::function<void()> function) = 0;

  /**
   * @brief Clears the queue of unstarted works.
   */
  virtual void clear() = 0;

  /**
   * @returns The size of work queue.
   */
  virtual std::size_t queue_size() const = 0;

  /**
   * @returns `(queue_size() == 0)`.
   */
  virtual bool is_queue_empty() const = 0;

  /**
   * @returns The threadpool size.
   */
  virtual std::size_t size() const = 0;

  /**
   * @brief Starts the threadpool.
   */
  virtual void start() = 0;

  /**
   * @brief Stops the threadpool.
   *
   * @see start().
   */
  virtual void stop() = 0;

  /**
   * @returns `true` if the threadpool is working, or `false` otherwise.
   */
  virtual bool is_working() const = 0;

private:
  friend detail::iSimple_threadpool;

  Simple_threadpool() = default;
};

} // namespace dmitigr::thread

#ifdef DMITIGR_UTIL_HEADER_ONLY
#include "dmitigr/util/thread.cpp"
#endif

#endif  // DMITIGR_UTIL_THREAD_HPP
