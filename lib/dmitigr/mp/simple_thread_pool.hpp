// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or mp.hpp

#ifndef DMITIGR_MP_SIMPLE_THREAD_POOL_HPP
#define DMITIGR_MP_SIMPLE_THREAD_POOL_HPP

#include <dmitigr/base/debug.hpp>

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>

namespace dmitigr::mp {

/**
 * @brief Simple threadpool.
 */
class Simple_threadpool final {
public:
  /**
   * @brief The destructor.
   */
  ~Simple_threadpool()
  {
    stop();
  }

  /// @name Constructors
  /// @{

  /// Non copy-consructible.
  Simple_threadpool(const Simple_threadpool&) = delete;

  /// Non copy-assignable.
  Simple_threadpool& operator=(const Simple_threadpool&) = delete;

  /// Non move-constructible.
  Simple_threadpool(Simple_threadpool&&) = delete;

  /// Non move-assignable.
  Simple_threadpool& operator=(Simple_threadpool&&) = delete;

  /**
   * @brief Constructs the threadpool with size of `size`.
   *
   * @par Requires
   * `(size > 0 && queue_max_size > 0)`.
   */
  explicit Simple_threadpool(const std::size_t size)
    : workers_{size}
  {
    DMITIGR_REQUIRE(size > 0, std::invalid_argument);
    DMITIGR_ASSERT(workers_.size() == size);
  }

  /// @}

  /**
   * @brief Submit the function to run on the threadpool.
   *
   * @par Requires
   * `(function)`.
   */
  void submit(std::function<void()> function)
  {
    DMITIGR_REQUIRE(function, std::invalid_argument);
    const std::lock_guard lg{queue_mutex_};
    queue_.push(std::move(function));
    state_changed_.notify_one();
  }

  /**
   * @brief Clears the queue of unstarted works.
   */
  void clear()
  {
    const std::lock_guard lg{queue_mutex_};
    queue_.clear();
  }

  /**
   * @returns The size of work queue.
   */
  std::size_t queue_size() const
  {
    const std::lock_guard lg{queue_mutex_};
    return queue_.size();
  }

  /**
   * @returns `(queue_size() == 0)`.
   */
  bool is_queue_empty() const
  {
    const std::lock_guard lg{queue_mutex_};
    return queue_.empty();
  }

  /**
   * @returns The threadpool size.
   */
  std::size_t size() const
  {
    const std::lock_guard lg{work_mutex_};
    return workers_.size();
  }

  /**
   * @brief Starts the threadpool.
   */
  void start()
  {
    const std::lock_guard lg{work_mutex_};

    if (is_working_)
      return;

    is_working_ = true;
    for (auto& worker : workers_)
      worker = std::thread{&Simple_threadpool::do_work, this};
    state_changed_.notify_all();
  }

  /**
   * @brief Stops the threadpool.
   *
   * @see start().
   */
  void stop()
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

  /**
   * @returns `true` if the threadpool is working, or `false` otherwise.
   */
  bool is_working() const
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

} // namespace dmitigr::mp

#endif  // DMITIGR_MP_SIMPLE_THREAD_POOL_HPP
