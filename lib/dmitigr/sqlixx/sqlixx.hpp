// -*- C++ -*-
// Copyright (C) 2021 Dmitry Igrishin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef DMITIGR_SQLIXX_SQLIXX_HPP
#define DMITIGR_SQLIXX_SQLIXX_HPP

#include <sqlite3.h>

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <exception>
#include <new>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <utility>

#if (defined(__clang__) && (__clang_major__ < 7)) || \
    (defined(__GNUG__)  && (__GNUC__ < 8))
#include <experimental/filesystem>
namespace dmitigr::sqlixx::detail {
namespace std_filesystem = std::experimental::filesystem;
}
#else
#include <filesystem>
namespace dmitigr::sqlixx::detail {
namespace std_filesystem = std::filesystem;
}
#endif

#define DMITIGR_SQLIXX_VERSION_MAJOR 0
#define DMITIGR_SQLIXX_VERSION_MINOR 1

namespace dmitigr::sqlixx {

/// @returns The version of library calculated as `(major * 1000 + minor)`.
constexpr int version() noexcept
{
  constexpr int major = DMITIGR_SQLIXX_VERSION_MAJOR;
  constexpr int minor = DMITIGR_SQLIXX_VERSION_MINOR;
  return major*1000 + minor; // 1.23 -> 1 * 1000 + 23 = 1023
}

/// A category of SQLite errors.
class Sqlite_error_category final : public std::error_category {
public:
  /// @returns The literal `dmitigr_sqlixx_sqlite_error`.
  const char* name() const noexcept override
  {
    return "dmitigr_sqlixx_sqlite_error";
  }

  /// @returns The string that describes the error condition denoted by `ev`.
  std::string message(int ev) const override
  {
    return std::string{name()}.append(" ").append(std::to_string(ev));
  }
};

/// The instance of type Sqlite_error_category.
inline Sqlite_error_category sqlite_error_category;

/// An exception.
class Exception final : public std::system_error {
public:
  /// The constructor.
  explicit Exception(const int ev)
    : system_error{ev, sqlite_error_category}
  {}

  /// @overload
  Exception(const int ev, const std::string& what)
    : system_error{ev, sqlite_error_category, what}
  {}
};

// =============================================================================

/// A data.
template<typename T, unsigned char E>
struct Data final {
  static_assert((E == 0) || (E == SQLITE_UTF8) ||
    (E == SQLITE_UTF16LE) || (E == SQLITE_UTF16BE) || (E == SQLITE_UTF16),
    "invalid data encoding");

  /// An alias of deleter.
  using Deleter = sqlite3_destructor_type;

  /// The data type.
  using Type = T;

  /// The data size type.
  using Size = sqlite3_uint64;

  /// The data encoding.
  constexpr static unsigned char Encoding = E;

  /// The destructor.
  ~Data()
  {
    if (is_data_owner())
      deleter_(const_cast<T*>(data_));
  }

  /// The default constructor.
  Data() = default;

  /// The constructor.
  Data(const T* const data, const Size size,
    const Deleter deleter = SQLITE_STATIC) noexcept
    : data_{data}
    , size_{size}
    , deleter_{deleter}
  {}

  /// Non-copyable.
  Data(const Data&) = delete;

  /// Non-copyable.
  Data& operator=(const Data&) = delete;

  /// The move constructor.
  Data(Data&& rhs) noexcept
    : data_{rhs.data_}
    , size_{rhs.size_}
    , deleter_{rhs.deleter_}
  {
    rhs.data_ = {};
    rhs.size_ = {};
    rhs.deleter_ = SQLITE_STATIC;
  }

  /// The move assignment operator.
  Data& operator=(Data&& rhs) noexcept
  {
    if (this != &rhs) {
      Data tmp{std::move(rhs)};
      swap(tmp);
    }
    return *this;
  }

  /// The swap operation.
  void swap(Data& other) noexcept
  {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(deleter_, other.deleter_);
  }

  /// @returns The released data.
  T* release() noexcept
  {
    auto* const result = data_;
    deleter_ = SQLITE_STATIC;
    assert(!is_data_owner());
    Data{}.swap(*this);
    return result;
  }

  /// @returns The data.
  const T* data() const noexcept { return data_; }

  /// @returns The data size.
  Size size() const noexcept { return size_; }

  /// @returns The deleter.
  Deleter deleter() const noexcept { return deleter_; }

  /// @returns `true` if this instance is owns the data.
  bool is_data_owner() const noexcept
  {
    return deleter_ && (deleter_ != SQLITE_STATIC) && (deleter_ != SQLITE_TRANSIENT);
  }

  /// The data.
  const T* data_{};

  /// The data size.
  Size size_{};

  /// The deleter.
  Deleter deleter_{SQLITE_STATIC};
};

/// An alias of Blob type.
using Blob = Data<void, 0>;

/// An alias of UTF8 text type.
using Text_utf8 = Data<char, SQLITE_UTF8>;

/// An alias of UTF16 text type.
using Text_utf16 = Data<char, SQLITE_UTF16>;

/// An alias of UTF16LE text type.
using Text_utf16le = Data<char, SQLITE_UTF16LE>;

/// An alias of UTF16BE text type.
using Text_utf16be = Data<char, SQLITE_UTF16BE>;

// =============================================================================

namespace detail {
inline void check_bind(sqlite3_stmt* const handle, const int r)
{
  assert(handle);
  if (r != SQLITE_OK)
    throw Exception{r, std::string{"cannot bind a prepared statement parameter"}
      .append(" (").append(sqlite3_errmsg(sqlite3_db_handle(handle))).append(")")};
}
} // namespace detail

/// The centralized "namespace" for data conversions.
template<typename, typename = void> struct Conversions;

/// The implementation of `int` conversions.
template<>
struct Conversions<int> final {
  static void bind(sqlite3_stmt* const handle, const int index, const int value)
  {
    detail::check_bind(handle, sqlite3_bind_int(handle, index, value));
  }

  static int result(sqlite3_stmt* const handle, const int index)
  {
    return sqlite3_column_int(handle, index);
  }
};

/// The implementation of `sqlite3_int64` conversions.
template<>
struct Conversions<sqlite3_int64> final {
  static void bind(sqlite3_stmt* const handle, const int index, const sqlite3_int64 value)
  {
    detail::check_bind(handle, sqlite3_bind_int64(handle, index, value));
  }

  static sqlite3_int64 result(sqlite3_stmt* const handle, const int index)
  {
    return sqlite3_column_int64(handle, index);
  }
};

/// The implementation of `double` conversions.
template<>
struct Conversions<double> final {
  static void bind(sqlite3_stmt* const handle, const int index, const double value)
  {
    detail::check_bind(handle, sqlite3_bind_double(handle, index, value));
  }

  static double result(sqlite3_stmt* const handle, const int index)
  {
    return sqlite3_column_double(handle, index);
  }
};

/// The implementation of `Data` conversions.
template<typename T, unsigned char E>
struct Conversions<Data<T, E>> final {
  template<typename B>
  static std::enable_if_t<std::is_same_v<std::decay_t<B>, Data<T, E>>>
  bind(sqlite3_stmt* const handle, const int index, B&& value)
  {
    const typename Data<T, E>::Deleter destr = [&value]
    {
      if constexpr (std::is_rvalue_reference_v<B&&>) {
        if (value.is_data_owner()) {
          const auto result = value.deleter_;
          value = {};
          assert(!value.is_data_owner());
          return result;
        } else
          return SQLITE_STATIC;
      } else
        return SQLITE_STATIC;
    }();

    const int br = [&]
    {
      if constexpr (E == 0)
        return sqlite3_bind_blob64(handle, index, value.data(), value.size(), destr);
      else
        return sqlite3_bind_text64(handle, index, value.data(), value.size(), destr, E);
    }();
    detail::check_bind(handle, br);
  }

  static Data<T, E> result(sqlite3_stmt* const handle, const int index)
  {
    static_assert((E == 0) || (E == SQLITE_UTF8) || (E == SQLITE_UTF16),
      "SQLite only provides sqlite3_column_text() and sqlite3_column_text16()");
    using R = Data<T, E>;
    if constexpr (E == 0)
      return R{sqlite3_column_blob(handle, index),
        static_cast<typename R::Size>(sqlite3_column_bytes(handle, index))};
    else if (E == SQLITE_UTF8)
      return R{reinterpret_cast<const typename R::Type*>(sqlite3_column_text(handle, index)),
        static_cast<typename R::Size>(sqlite3_column_bytes(handle, index))};
    else // SQLITE_UTF16
      return R{reinterpret_cast<const typename R::Type*>(sqlite3_column_text16(handle, index)),
        static_cast<typename R::Size>(sqlite3_column_bytes16(handle, index))};
  }
};

/// The implementation of `std::string` and `std::string_view` conversions.
template<typename T>
struct Conversions<T,
  std::enable_if_t<
    std::is_same_v<T, std::string> ||
    std::is_same_v<T, std::string_view>>> final {
  template<typename S>
  static std::enable_if_t<std::is_same_v<std::decay_t<S>, T>>
  bind(sqlite3_stmt* const handle, const int index, S&& value)
  {
    const auto destr = std::is_rvalue_reference_v<S&&> ? SQLITE_TRANSIENT : SQLITE_STATIC;
    detail::check_bind(handle, sqlite3_bind_text64(handle,
      index, value.data(), value.size(), destr, SQLITE_UTF8));
  }

  static T result(sqlite3_stmt* const handle, const int index)
  {
    return T{reinterpret_cast<const char*>(sqlite3_column_text(handle, index)),
        static_cast<typename T::size_type>(sqlite3_column_bytes(handle, index))};
  }
};

/// The implementation of `std::optional<T>` conversions.
template<typename T>
struct Conversions<std::optional<T>> final {
  template<typename O>
  static std::enable_if_t<std::is_same_v<std::decay_t<O>, std::optional<T>>>
  bind(sqlite3_stmt* const handle, const int index, O&& value)
  {
    if (value) {
      if constexpr (std::is_rvalue_reference_v<O&&>)
        bind(handle, index, std::move(*value));
      else
        bind(handle, index, *value);
    } else
      detail::check_bind(handle, sqlite3_bind_null(handle, index));
  }

  static std::optional<T> result(sqlite3_stmt* const handle, const int index)
  {
    if (sqlite3_column_type(handle, index) != SQLITE_NULL)
      return Conversions<T>::result(handle, index);
    else
      return std::nullopt;
  }
};

// =============================================================================

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

// =============================================================================

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
    assert(ref);
    if (const int r = sqlite3_open_v2(ref, &handle_, flags, nullptr); r != SQLITE_OK) {
      if (handle_)
        throw Exception{r, sqlite3_errmsg(handle_)};
      else
        throw std::bad_alloc{};
    }
    assert(handle_);
  }

  /// @overload
  Connection(const std::string& ref, const int flags)
    : Connection{ref.c_str(), flags}
  {}

  /// @overload
  Connection(const detail::std_filesystem::path& path, const int flags)
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
    assert(handle_);
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
  bool is_transaction_active() const noexcept
  {
    assert(handle_);
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

#endif  // DMITIGR_SQLIXX_SQLIXX_HPP
