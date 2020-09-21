// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#ifndef DMITIGR_WS_TIMER_HPP
#define DMITIGR_WS_TIMER_HPP

#include "dmitigr/ws/types_fwd.hpp"

#include <chrono>
#include <functional>

namespace dmitigr::ws {

/**
 * @brief A timer.
 *
 * Timers are called on the event-loop. Thus they can be used for graceful
 * shutdown for instance.
 */
class Timer {
public:
  /**
   * @brief An alias of a function to handle triggering a timer.
   *
   * @see start().
   */
  using Handler = std::function<void()>;

  /// The destructor.
  virtual ~Timer() = default;

  /**
   * @brief Sets the handler to be invoked by this timer.
   *
   * @par Requires
   * `handler`.
   *
   * @returns The reference to this instance.
   */
  virtual Timer& set_handler(Handler handler) = 0;

  /**
   * @brief Starts the timer.
   *
   * @param timeout A delay before the first invocation of handler. If `timeout`
   * is zero, the handler invokes on the next event loop iteration.
   * @param repeat A repeat interval. If `repeat` is non-zero, the handler
   * invokes first after `timeout` and then repeatedly after the specified value.
   *
   * @par Requires
   * `((0 < timeout && timeout < 2147483648) &&
   *   (0 <= repeat && repeat < 2147483648))`.
   *
   * @see stop(), is_active().
   */
  virtual void start(std::chrono::milliseconds timeout, std::chrono::milliseconds repeat) = 0;

  /**
   * @brief Stops the timer, the handler will not be called anymore.
   *
   * @see start(), is_active().
   */
  virtual void stop() = 0;

  /**
   * @returns `true` if the timer has been started.
   *
   * @see start(), stop().
   */
  virtual bool is_active() const = 0;

private:
  friend detail::iTimer;

  Timer() = default;
};

} // namespace dmitigr::ws

#ifdef DMITIGR_WS_HEADER_ONLY
#include "dmitigr/ws/timer.cpp"
#endif

#endif  // DMITIGR_WS_TIMER_HPP
