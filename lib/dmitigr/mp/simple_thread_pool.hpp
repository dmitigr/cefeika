// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or mp.hpp

#ifndef DMITIGR_MP_SIMPLE_THREAD_POOL_HPP
#define DMITIGR_MP_SIMPLE_THREAD_POOL_HPP

#include <dmitigr/util/debug.hpp>

#include <cassert>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

namespace dmitigr::mp {

/**
 * @brief Simple thread pool.
 */
class Simple_thread_pool final {
public:
  /**
   * @brief The destructor.
   */
  ~Simple_thread_pool()
  {
    stop();
  }

  /// @name Constructors
  /// @{

  /// Non copy-consructible.
  Simple_thread_pool(const Simple_thread_pool&) = delete;

  /// Non copy-assignable.
  Simple_thread_pool& operator=(const Simple_thread_pool&) = delete;

  /// Non move-constructible.
  Simple_thread_pool(Simple_thread_pool&&) = delete;

  /// Non move-assignable.
  Simple_thread_pool& operator=(Simple_thread_pool&&) = delete;

  /**
   * @brief Constructs the thread pool with size of `size`.
   *
   * @par Requires
   * `(size > 0 && queue_max_size > 0)`.
   */
  explicit Simple_thread_pool(const std::size_t size, std::string name = {})
    : name_{std::move(name)}
    , workers_{size}
  {
    DMITIGR_REQUIRE(size > 0, std::invalid_argument);
    DMITIGR_ASSERT(workers_.size() == size);
  }

  /// @}

  /**
   * @brief Submit the function to run on the thread pool.
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
   * @returns The thread pool size.
   */
  std::size_t size() const
  {
    const std::lock_guard lg{work_mutex_};
    return workers_.size();
  }

  /**
   * @brief Starts the thread pool.
   */
  void start()
  {
    const std::lock_guard lg{work_mutex_};

    if (is_running_)
      return;

    is_running_ = true;
    for (auto& worker : workers_)
      worker = std::thread{&Simple_thread_pool::wait_and_run, this};
    state_changed_.notify_all();
  }

  /**
   * @brief Stops the thread pool.
   *
   * @see start().
   */
  void stop()
  {
    const std::lock_guard lg{work_mutex_};

    if (!is_running_)
      return;

    is_running_ = false;
    state_changed_.notify_all();
    for (auto& worker : workers_) {
      if (worker.joinable())
        worker.join();
    }
  }

  /**
   * @returns `true` if the thread pool is running, or `false` otherwise.
   */
  bool is_running() const noexcept
  {
    const std::lock_guard lg{work_mutex_};
    return is_running_;
  }

private:
  class Queue final : public std::queue<std::function<void()>> {
  public:
    void clear() { c.clear(); }
  };

  const std::string name_;

  std::condition_variable state_changed_;

  mutable std::mutex queue_mutex_;
  Queue queue_;

  mutable std::mutex work_mutex_;
  std::vector<std::thread> workers_;
  bool is_running_{};

  void wait_and_run() noexcept
  {
    while (true) {
      try {
        std::function<void()> func;
        {
          std::unique_lock lk{queue_mutex_};
          state_changed_.wait(lk, [this]{ return !queue_.empty() || !is_running_; });
          if (is_running_) {
            assert(!queue_.empty());
            func = std::move(queue_.front());
            assert(static_cast<bool>(func));
            queue_.pop();
          } else
            return;
        }
        func();
      } catch (const std::exception& e) {
        log_error(e.what());
      } catch (...) {
        log_error("unknown error");
      }
    }
  }

  void log_error(const char* const what) const noexcept
  {
    assert(what);
    std::clog << "dmitigr::mp::Simple_thread_pool ";
    if (!name_.empty())
      std::clog << name_ << " ";
    std::clog << "(thread " << std::this_thread::get_id() << "): " << what << "\n";
  }
};

} // namespace dmitigr::mp

#endif  // DMITIGR_MP_SIMPLE_THREAD_POOL_HPP
