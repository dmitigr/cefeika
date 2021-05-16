// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include "timer.hpp"
#include "../assert.hpp"
#include "../thirdparty/usockets/libusockets.h"

#include <uv.h> // instead of <internal/eventing/libuv.h>

#include <limits>
#include <memory>

namespace std {

template<> struct default_delete<us_timer_t> final {
  void operator()(::us_timer_t* const ptr) const
  {
    us_timer_close(ptr);
  }
};

} // namespace std

namespace dmitigr::ws::detail {

/// The Timer implementation.
class iTimer final : public Timer {
public:
  explicit iTimer(us_loop_t* const loop)
    : rep_{us_create_timer(loop, 0, sizeof(this))}
  {
    auto** const ext = ext__(rep_.get());
    *ext = this;
  }

  iTimer(const iTimer&) = delete;
  iTimer& operator=(const iTimer&) = delete;

  iTimer(iTimer&& rhs)
    : rep_{std::move(rhs.rep_)}
    , handler_{std::move(rhs.handler_)}
  {
    auto** const ext = ext__(rep_.get());
    *ext = this;
  }

  iTimer& operator=(iTimer&& rhs)
  {
    if (this != &rhs) {
      rep_ = std::move(rhs.rep_);
      handler_ = std::move(rhs.handler_);
    }
    return *this;
  }

  iTimer& set_handler(Handler handler) override
  {
    DMITIGR_CHECK_ARG(handler);
    handler_ = std::move(handler);
    return *this;
  }

  void start(const std::chrono::milliseconds timeout, const std::chrono::milliseconds repeat) override
  {
    DMITIGR_CHECK_RANGE(0 < timeout.count() && timeout.count() <= std::numeric_limits<int>::max());
    DMITIGR_CHECK_RANGE(0 <= repeat.count() && repeat.count() <= std::numeric_limits<int>::max());
    auto** const ext = ext__(rep_.get());
    DMITIGR_ASSERT(ext);
    auto* const self = *ext;
    DMITIGR_ASSERT(self == this);
    DMITIGR_ASSERT(self->handler_);
    us_timer_set(rep_.get(), &cb__, static_cast<int>(timeout.count()), static_cast<int>(repeat.count()));
  }

  void stop() override
  {
    us_timer_set(rep_.get(), nullptr, 0, 0);
  }

  bool is_active() const override
  {
    const auto* const us_loop = us_timer_loop(rep_.get());
    const auto* const uv_loop = reinterpret_cast<const uv_loop_t*>(us_loop);
    return uv_is_active(reinterpret_cast<const uv_handle_t*>(uv_loop));
  }

private:
  std::unique_ptr<us_timer_t> rep_;
  Handler handler_;

  static void cb__(us_timer_t* const timer)
  {
    DMITIGR_ASSERT(timer);
    auto** const ext = ext__(timer);
    auto* const self = *ext;
    DMITIGR_ASSERT(self);
    DMITIGR_ASSERT(self->handler_);
    self->handler_();
  }

  static iTimer** ext__(us_timer_t* const timer)
  {
    DMITIGR_ASSERT(timer);
    return static_cast<iTimer**>(us_timer_ext(timer));
  }
};

} // namespace dmitigr::ws::detail
