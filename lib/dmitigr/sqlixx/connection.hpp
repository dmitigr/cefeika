// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or sqlixx.hpp

#ifndef DMITIGR_SQLIXX_CONNECTION_HPP
#define DMITIGR_SQLIXX_CONNECTION_HPP

#include "statement.hpp"
#include "../misc/assert.hpp"
#include "../misc/filesystem.hpp"

#include <sqlite3.h>

#include <cstdio>
#include <exception>
#include <new>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace dmitigr::sqlixx {

/// A database(-s) connection.
class Connection final {
public:
  /// The destructor.
  ~Connection()
  {
    try {
      close();
    } catch(const std::exception& e) {
      std::fprintf(stderr, "%s\n", e.what());
    } catch(...) {}
  }

  /// The constructor.
  explicit Connection(sqlite3* handle = {})
    : handle_{handle}
  {}

  /**
   * @brief The constructor.
   *
   * @param ref Path to a file or URI.
   *
   * @see https://www.sqlite.org/uri.html
   */
  Connection(const char* const ref, const int flags)
  {
    DMITIGR_CHECK_ARG(ref);
    if (const int r = sqlite3_open_v2(ref, &handle_, flags, nullptr); r != SQLITE_OK) {
      if (handle_)
        throw Exception{r, sqlite3_errmsg(handle_)};
      else
        throw std::bad_alloc{};
    }
    DMITIGR_ASSERT(handle_);
  }

  /// @overload
  Connection(const std::string& ref, const int flags)
    : Connection{ref.c_str(), flags}
  {}

  /// @overload
  Connection(const std::filesystem::path& path, const int flags)
    : Connection{path.c_str(), flags}
  {}

  /// Non-copyable.
  Connection(const Connection&) = delete;

  /// Non-copyable.
  Connection& operator=(const Connection&) = delete;

  /// The move constructor.
  Connection(Connection&& rhs) noexcept
    : handle_{rhs.handle_}
  {
    rhs.handle_ = {};
  }

  /// The move assignment operator.
  Connection& operator=(Connection&& rhs) noexcept
  {
    if (this != &rhs) {
      Connection tmp{std::move(rhs)};
      swap(tmp);
    }
    return *this;
  }

  /// The swap operation.
  void swap(Connection& other) noexcept
  {
    std::swap(handle_, other.handle_);
  }

  /// @returns The guarded handle.
  sqlite3* handle() const noexcept
  {
    return handle_;
  }

  /// @returns `true` if this object keeps handle, or `false` otherwise.
  explicit operator bool() const noexcept
  {
    return handle_;
  }

  /// @returns The released handle.
  sqlite3* release() noexcept
  {
    auto* const result = handle_;
    handle_ = {};
    return result;
  }

  /// Closes the database connection.
  void close()
  {
    if (const int r = sqlite3_close(handle_); r != SQLITE_OK)
      throw Exception{r, "failed to close a database connection"};
    else
      handle_ = {};
  }

  /**
   * @returns An instance of type Statement.
   *
   * @see Statement::Statement().
   */
  Statement prepare(const std::string_view sql, const unsigned int flags = 0)
  {
    return Statement{handle_, sql, flags};
  }

  /**
   * Executes the `sql`.
   *
   * @see Statement::execute().
   */
  template<typename F, typename ... Types>
  std::enable_if_t<detail::Execute_callback_traits<F>::is_valid>
  execute(F&& callback, const std::string_view sql, Types&& ... values)
  {
    DMITIGR_CHECK(handle_);
    prepare(sql).execute(std::forward<F>(callback), std::forward<Types>(values)...);
  }

  /// @overload
  template<typename ... Types>
  void execute(const std::string_view sql, Types&& ... values)
  {
    execute([](const auto&){ return true; }, sql, std::forward<Types>(values)...);
  }

  /**
   * @returns `true` if this connection is not in autocommit mode, or `false`
   * otherwise. Autocommit mode is disabled by a `BEGIN` command and re-enabled
   * by a `COMMIT` or `ROLLBACK` commands.
   */
  bool is_transaction_active() const
  {
    DMITIGR_CHECK(handle_);
    return (sqlite3_get_autocommit(handle_) == 0);
  }

  /**
   * @brief Calls the `callback`.
   *
   * If the call of `callback` fails with exception and there is an active
   * transaction, an attempt is made to rollback this transaction. If this
   * attempt is successful, the exception thrown by `callback` is rethrown
   * as is, otherwise the exception thrown by `callback` is rethrown with
   * a nested exception of type `Exception`.
   *
   * @tparam F A type of callback.
   */
  template<typename F>
  auto with_rollback_on_error(F&& callback)
  {
    try {
      return callback();
    } catch (...) {
      if (is_transaction_active()) {
        int rollback_failure{SQLITE_OK};
        try {
          execute("rollback");
        } catch (const Exception& e) {
          rollback_failure = e.code().value();
        } catch (...) {
          rollback_failure = SQLITE_ERROR;
        }
        if (rollback_failure == SQLITE_OK)
          throw;
        else
          std::throw_with_nested(Exception{rollback_failure});
      } else
        throw;
    }
  }

private:
  sqlite3* handle_{};
};

} // namespace dmitigr::sqlixx

#endif  // DMITIGR_SQLIXX_CONNECTION_HPP
