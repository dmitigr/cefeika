// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or sqlixx.hpp

#ifndef DMITIGR_SQLIXX_STATEMENT_HPP
#define DMITIGR_SQLIXX_STATEMENT_HPP

#include "conversions.hpp"

#include <sqlite3.h>

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <new>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace dmitigr::sqlixx {

class Statement;

namespace detail {
template<typename F, typename = void>
struct Execute_callback_traits final {
  constexpr static bool is_valid = false;
};

template<typename F>
struct Execute_callback_traits<F,
  std::enable_if_t<std::is_invocable_v<F, const Statement&>>> final {
  using Result = std::invoke_result_t<F, const Statement&>;
  constexpr static bool is_result_void = std::is_same_v<Result, void>;
  constexpr static bool is_valid =
    std::is_invocable_r_v<bool, F, const Statement&> || is_result_void;
  constexpr static bool has_error_parameter = false;
};

template<typename F>
struct Execute_callback_traits<F,
  std::enable_if_t<std::is_invocable_v<F, const Statement&, int>>> final {
  using Result = std::invoke_result_t<F, const Statement&, int>;
  constexpr static bool is_result_void = std::is_same_v<Result, void>;
  constexpr static bool is_valid =
    std::is_invocable_r_v<bool, F, const Statement&, int> || is_result_void;
  constexpr static bool has_error_parameter = true;
};
} // namespace detail

/// A prepared statement.
class Statement final {
public:
  /// The destructor.
  ~Statement()
  {
    try {
      close();
    } catch(const std::exception& e) {
      std::fprintf(stderr, "%s\n", e.what());
    } catch(...) {}
  }

  /// The constructor.
  explicit Statement(sqlite3_stmt* const handle = {})
    : handle_{handle}
  {}

  /// @overload
  Statement(sqlite3* const handle, const std::string_view sql,
    const unsigned int flags = 0)
  {
    assert(handle);
    if (const int r = sqlite3_prepare_v3(handle, sql.data(), sql.size(), flags,
        &handle_, nullptr); r != SQLITE_OK)
      throw Exception{r, std::string{"cannot prepare statement "}.append(sql)
        .append(" (").append(sqlite3_errmsg(handle)).append(")")};
    assert(handle_);
  }

  /// Non-copyable.
  Statement(const Statement&) = delete;

  /// Non-copyable.
  Statement& operator=(const Statement&) = delete;

  /// The move constructor.
  Statement(Statement&& rhs) noexcept
    : handle_{rhs.handle_}
  {
    rhs.handle_ = {};
  }

  /// The move assignment operator.
  Statement& operator=(Statement&& rhs) noexcept
  {
    if (this != &rhs) {
      Statement tmp{std::move(rhs)};
      swap(tmp);
    }
    return *this;
  }

  /// The swap operation.
  void swap(Statement& other) noexcept
  {
    std::swap(handle_, other.handle_);
  }

  /// @returns The underlying handle.
  sqlite3_stmt* handle() const noexcept
  {
    return handle_;
  }

  /// @returns `true` if this object keeps handle, or `false` otherwise.
  explicit operator bool() const noexcept
  {
    return handle_;
  }

  /// @returns The released handle.
  sqlite3_stmt* release() noexcept
  {
    auto* const result = handle_;
    handle_ = {};
    return result;
  }

  /// Closes the prepared statement.
  void close()
  {
    if (const int r = sqlite3_finalize(handle_); r != SQLITE_OK)
      throw Exception{r, "cannot close a prepared statement"};
    else
      handle_ = {};
  }

  // ---------------------------------------------------------------------------

  /// @name Parameters
  /// @remarks Parameter indexes starts from zero!
  /// @{

  /// @returns The number of parameters.
  int parameter_count() const
  {
    assert(handle_);
    return sqlite3_bind_parameter_count(handle_);
  }

  /// @returns The parameter index, or -1 if no parameter `name` presents.
  int parameter_index(const char* const name) const
  {
    assert(handle_ && name);
    return sqlite3_bind_parameter_index(handle_, name) - 1;
  }

  /**
   * @returns The parameter index.
   */
  int parameter_index_throw(const char* const name) const
  {
    assert(handle_ && name);
    const int index = parameter_index(name);
#ifndef NDEBUG
    if (index < 0)
      throw std::logic_error{std::string{"no parameter with name "}.append(name)};
#endif
    return index;
  }

  /// @returns The name of the parameter by the `index`.
  std::string parameter_name(const int index) const noexcept
  {
    assert(handle_ && (index < parameter_count()));
    return sqlite3_column_name(handle_, index + 1);
  }

  /// Binds all the parameters with NULL.
  void bind_null()
  {
    assert(handle_);
    detail::check_bind(handle_, sqlite3_clear_bindings(handle_));
  }

  /// Binds the parameter of the specified index with NULL.
  void bind_null(const int index)
  {
    assert(handle_ && (index < parameter_count()));
    detail::check_bind(handle_, sqlite3_bind_null(handle_, index + 1));
  }

  /// @overload
  void bind_null(const char* const name)
  {
    bind_null(parameter_index_throw(name));
  }

  /**
   * @brief Binds the parameter of the specified index with `value`.
   *
   * @remarks `value` is assumed to be UTF-8 encoded.
   */
  void bind(const int index, const char* const value)
  {
    assert(handle_ && (index < parameter_count()));
    detail::check_bind(handle_, sqlite3_bind_text(handle_, index + 1, value, -1, SQLITE_STATIC));
  }

  /// @overload
  void bind(const char* const name, const char* const value)
  {
    bind(parameter_index_throw(name), value);
  }

  /**
   * @brief Binds the parameter of the specified index with the value of type `T`.
   *
   * @param index A zero-based index of the parameter.
   * @param value A value to bind. If this parameter is `lvalue`, then it's assumed
   * that the value is a constant and does not need to be copied. If this parameter
   * is `rvalue`, then it's assumed to be destructed after this function returns, so
   * SQLite is required to make a private copy of the value before return.
   */
  template<typename T>
  void bind(const int index, T&& value)
  {
    assert(handle_ && (index < parameter_count()));
    using U = std::decay_t<T>;
    Conversions<U>::bind(handle_, index + 1, std::forward<T>(value));
  }

  /// @overload
  template<typename T>
  void bind(const char* const name, T&& value)
  {
    bind(parameter_index_throw(name), std::forward<T>(value));
  }

  /**
   * @brief Binds parameters by indexes in range [0, sizeof ... (values)).
   *
   * In other words:
   * @code bind_many(value1, value2, value3) @endcode
   * equivalently to
   * @code (bind(0, value1), bind(1, value1), bind(2, value2)) @endcode
   *
   * @see bind().
   */
  template<typename ... Types>
  void bind_many(Types&& ... values)
  {
    bind_many__(std::make_index_sequence<sizeof ... (Types)>{}, std::forward<Types>(values)...);
  }

  /// @}

  // ---------------------------------------------------------------------------

  /// @name Execution
  /// @{

  /**
   * @brief Executes the prepared statement.
   *
   * @param callback A function to be called for each retrieved row. The callback:
   *   -# can be defined with a parameter of type `const Statement&`. The exception
   *   will be thrown on error in this case.
   *   -# can be defined with two parameters of type `const Statement&` and `int`.
   *   In case of error an error code will be passed as the second argument of
   *   the callback instead of throwing exception and execution will be stopped
   *   after callback returns. In case of success, `0` will be passed as the second
   *   argument of the callback.
   *   -# can return a value convertible to `bool` to indicate should the execution
   *   to be continued after the callback returns or not;
   *   -# can return `void` to indicate that execution must be proceed until a
   *   completion or an error.
   * @param values Values to bind with parameters. Binding will take place if this
   * method is never been called before for this instance or if the last value it
   * return was `SQLITE_DONE`.
   *
   * @remarks If the last value of this method was `SQLITE_DONE` then `reset()`
   * will be called automatically.
   *
   * @returns The result of `sqlite3_step()`.
   *
   * @see reset().
   */
  template<typename F, typename ... Types>
  std::enable_if_t<detail::Execute_callback_traits<F>::is_valid, int>
  execute(F&& callback, Types&& ... values)
  {
    assert(handle_);

    if (last_step_result_ == SQLITE_DONE)
      reset();

    if (last_step_result_ < 0 || last_step_result_ == SQLITE_DONE)
      bind_many(std::forward<Types>(values)...);

    while (true) {
      using Traits = detail::Execute_callback_traits<F>;
      switch (last_step_result_ = sqlite3_step(handle_)) {
      case SQLITE_ROW:
        if constexpr (!Traits::is_result_void) {
          if constexpr (!Traits::has_error_parameter) {
            if (!callback(static_cast<const Statement&>(*this)))
              return last_step_result_;
          } else {
            if (!callback(static_cast<const Statement&>(*this), last_step_result_))
              return last_step_result_;
          }
        } else {
          if constexpr (!Traits::has_error_parameter)
            callback(static_cast<const Statement&>(*this));
          else
            callback(static_cast<const Statement&>(*this), last_step_result_);
        }
        continue;
      case SQLITE_DONE:
        return last_step_result_;
      default:
        if constexpr (Traits::has_error_parameter) {
          callback(static_cast<const Statement&>(*this), last_step_result_);
          return last_step_result_;
        } else
          throw Exception{last_step_result_, std::string{"failed to execute a prepared statement"}
            .append(" (").append(sqlite3_errmsg(sqlite3_db_handle(handle_))).append(")")};
      }
    }
  }

  /// @overload
  template<typename ... Types>
  int execute(Types&& ... values)
  {
    return execute([](const auto&){ return true; }, std::forward<Types>(values)...);
  }

  /// Resets the statement back to its initial state, ready to be executed.
  int reset()
  {
    return sqlite3_reset(handle_);
  }

  /// @}

  // ---------------------------------------------------------------------------

  /// @name Result
  /// @{

  /// @returns The number of columns.
  int column_count() const
  {
    assert(handle_);
    return sqlite3_column_count(handle_);
  }

  /// @returns The column index, or -1 if no column `name` presents.
  int column_index(const char* const name) const
  {
    assert(handle_ && name);
    const int count = column_count();
    for (int i = 0; i < count; ++i) {
      if (const char* const nm = sqlite3_column_name(handle_, i)) {
        if (!std::strcmp(name, nm))
          return i;
      } else
        throw std::bad_alloc{};
    }
    return -1;
  }

  /**
   * @returns The column index.
   */
  int column_index_throw(const char* const name) const
  {
    assert(handle_ && name);
    const int index = column_index(name);
#ifndef NDEBUG
    if (index < 0)
      throw std::logic_error{std::string{"no column with name "}.append(name)};
#endif
    return index;
  }

  /// @returns The name of the column by the `index`.
  std::string column_name(const int index) const noexcept
  {
    assert(handle_ && (index < column_count()));
    return sqlite3_column_name(handle_, index);
  }

  /// @overload
  template<typename T>
  T result(const int index) const
  {
    assert(handle_ && (index < column_count()));
    using U = std::decay_t<T>;
    return Conversions<U>::result(handle_, index);
  }

  /// @overload
  template<typename T>
  T result(const char* const name) const
  {
    return result<T>(column_index_throw(name));
  }

  /// @}

private:
  int last_step_result_{-1};
  sqlite3_stmt* handle_{};

  template<std::size_t ... I, typename ... Types>
  void bind_many__(std::index_sequence<I...>, Types&& ... values)
  {
    (bind(static_cast<int>(I), std::forward<Types>(values)), ...);
  }
};

} // namespace dmitigr::sqlixx

#endif  // DMITIGR_SQLIXX_STATEMENT_HPP
