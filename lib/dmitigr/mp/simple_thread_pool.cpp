// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or mp.hpp

#include "dmitigr/mp/simple_thread_pool.hpp"
#include "dmitigr/mp/implementation_header.hpp"

#include "dmitigr/util/debug.hpp"

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

namespace dmitigr::mp::detail {

/**
 * @brief The implementation of simple threadpool.
 */
class iSimple_threadpool final : public Simple_threadpool {
public:
  /// Non copy-consructible.
  iSimple_threadpool(const iSimple_threadpool&) = delete;

  /// Non copy-assignable.
  iSimple_threadpool& operator=(const iSimple_threadpool&) = delete;

  /// Non move-constructible.
  iSimple_threadpool(iSimple_threadpool&&) = delete;

  /// Non move-assignable.
  iSimple_threadpool& operator=(iSimple_threadpool&&) = delete;

  /// @see Simple_threadpool::~Simple_threadpool().
  ~iSimple_threadpool() override
  {
    stop();
  }

  /// @see Simple_threadpool::make().
  explicit iSimple_threadpool(const std::size_t size)
    : workers_{size}
  {
    DMITIGR_REQUIRE(size > 0, std::invalid_argument);
    DMITIGR_ASSERT(workers_.size() == size);
  }

  /// @see Simple_threadpool::submit().
  void submit(std::function<void()> function) override
  {
    DMITIGR_REQUIRE(function, std::invalid_argument);
    const std::lock_guard lg{queue_mutex_};
    queue_.push(std::move(function));
    state_changed_.notify_one();
  }

  /// @see Simple_threadpool::clear().
  void clear() override
  {
    const std::lock_guard lg{queue_mutex_};
    queue_.clear();
  }

  /// @see Simple_threadpool::queue_size().
  std::size_t queue_size() const override
  {
    const std::lock_guard lg{queue_mutex_};
    return queue_.size();
  }

  /// @see Simple_threadpool::is_queue_empty().
  bool is_queue_empty() const override
  {
    const std::lock_guard lg{queue_mutex_};
    return queue_.empty();
  }

  /// @see Simple_threadpool::size().
  std::size_t size() const override
  {
    const std::lock_guard lg{work_mutex_};
    return workers_.size();
  }

  /// @see Simple_threadpool::start().
  void start() override
  {
    const std::lock_guard lg{work_mutex_};

    if (is_working_)
      return;

    is_working_ = true;
    for (auto& worker : workers_)
      worker = std::thread{&iSimple_threadpool::do_work, this};
    state_changed_.notify_all();
  }

  /// @see Simple_threadpool::stop().
  void stop() override
  {
    const std::lock_guard lg{work_mutex_};

    if (!is_working_)
      return;

    is_working_ = false;
    state_changed_.notify_all();
    for (auto& worker : workers_) {
      if (worker.joinable())
        worker.join();
    }
  }

  /// @see Simple_threadpool::is_working().
  bool is_working() const override
  {
    const std::lock_guard lg{work_mutex_};
    return is_working_;
  }

private:
  class Queue final : public std::queue<std::function<void()>> {
  public:
    void clear() { c.clear(); }
  };

  enum class Pool_status {
    ok,
    stopped
  };

  std::pair<std::function<void()>, Pool_status> next_work()
  {
    std::unique_lock lock{queue_mutex_};
    state_changed_.wait(lock, [this]{ return !queue_.empty() || !is_working_; });
    std::function<void()> work;
    if (is_working_) {
      DMITIGR_ASSERT(static_cast<bool>(queue_.front()));
      work = std::move(queue_.front());
      queue_.pop();
      return std::make_pair(std::move(work), Pool_status::ok);
    } else
      return std::make_pair(std::move(work), Pool_status::stopped);
  }

  void do_work()
  {
    while (true) {
      const auto [work, status] = next_work();
      switch (status) {
      case Pool_status::ok:
        work();
        break;
      case Pool_status::stopped:
        return;
      }
    }
  }

  std::condition_variable state_changed_;

  mutable std::mutex queue_mutex_;
  Queue queue_;

  mutable std::mutex work_mutex_;
  std::vector<std::thread> workers_;
  bool is_working_{};
};

} // namespace dmitigr::mp::detail

namespace dmitigr::mp {

DMITIGR_MP_INLINE std::unique_ptr<Simple_threadpool> Simple_threadpool::make(const std::size_t size)
{
  return std::make_unique<detail::iSimple_threadpool>(size);
}

} // namespace dmitigr::mp

#include "dmitigr/mp/implementation_footer.hpp"
