// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#ifndef DMITIGR_PGFE_CONNECTION_HPP
#define DMITIGR_PGFE_CONNECTION_HPP

#include "dmitigr/pgfe/basics.hpp"
#include "dmitigr/pgfe/completion.hpp"
#include "dmitigr/pgfe/connection_options.hpp"
#include "dmitigr/pgfe/data.hpp"
#include "dmitigr/pgfe/dll.hpp"
#include "dmitigr/pgfe/error.hpp"
#include "dmitigr/pgfe/notice.hpp"
#include "dmitigr/pgfe/notification.hpp"
#include "dmitigr/pgfe/pq.hpp"
#include "dmitigr/pgfe/prepared_statement.hpp"
#include "dmitigr/pgfe/row_conversions.hpp"
#include "dmitigr/pgfe/sql_string.hpp"
#include "dmitigr/pgfe/types_fwd.hpp"

#include <cassert>
#include <chrono>
#include <cstdint>
#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <queue>
#include <string>
#include <type_traits>
#include <vector>

namespace dmitigr::pgfe {

/**
 * @ingroup main
 *
 * @brief A connection to a PostgreSQL server.
 */
class Connection final {
public:
  /// An alias of Connection_options.
  using Options = Connection_options;

  /**
   * @returns A new instance of this class.
   *
   * @param options - the connection options. The value of `nullptr` means
   * default connection options.
   */
  explicit Connection(Options options = {})
    : options_{std::move(options)}
    , notice_handler_{&default_notice_handler}
  {}

  /// Non copy-constructible.
  Connection(const Connection&) = delete;

  /// Non copy-assignable.
  Connection& operator=(const Connection&) = delete;

  /// Move-constructible.
  Connection(Connection&& rhs) = default;

  /// Move-assignable.
  Connection& operator=(Connection&& rhs) = default;

  /// @name General observers
  /// @{

  /**
   * @returns The communication status.
   *
   * @see is_connected().
   */
  DMITIGR_PGFE_API Communication_status communication_status() const noexcept;

  /**
   * @returns `(communication_status() == Communication_status::connected)`.
   *
   * @see communication_status().
   */
  bool is_connected() const noexcept
  {
    return (communication_status() == Communication_status::connected);
  }

  /// @returns `true` if the connection secured by SSL.
  bool is_ssl_secured() const noexcept
  {
    return conn() ? ::PQsslInUse(conn()) : false;
  }

  /**
   * @returns The last reported server transaction block status, or
   * `std::nullopt` if unavailable.
   *
   * @see is_transaction_block_uncommitted().
   */
  DMITIGR_PGFE_API std::optional<Transaction_block_status> transaction_block_status() const noexcept;

  /**
   * @returns `(transaction_block_status() == Transaction_block_status::uncommitted`).
   *
   * @see transaction_block_status().
   */
  bool is_transaction_block_uncommitted() const noexcept
  {
    return (transaction_block_status() == Transaction_block_status::uncommitted);
  }

  /**
   * @returns The last registered time point when is_connected() started to return `true`, or
   * `std::nullopt` if the session wasn't started.
   */
  std::optional<std::chrono::system_clock::time_point> session_start_time() const noexcept
  {
    return session_start_time_;
  }

  /// @returns The connection options of this instance.
  const Connection_options* options() const noexcept
  {
    return &options_;
  }

  /**
   * @returns The last reported identifier of the server process, or
   * `std::nullopt` if unavailable.
   *
   * @see Notification::server_pid().
   */
  std::optional<std::int_fast32_t> server_pid() const noexcept
  {
    if (conn()) {
      if (const int pid = ::PQbackendPID(conn()))
        server_pid_ = pid;
    }
    return server_pid_;
  }

  ///@}

  // ---------------------------------------------------------------------------

  /// @name Communication
  /// @{

  /**
   * @brief Establishing the connection to a PostgreSQL server without blocking
   * on I/O.
   *
   * This function should be called repeatedly. Until communication_status()
   * becomes Communication_status::connected or Communication_status::failure loop
   * thus: if communication_status() returned Communication_status::establishment_reading,
   * wait until the socket is ready to read, then call connect_async() again; if
   * communication_status() returned Communication_status::establishment_writing, wait
   * until the socket ready to write, then call connect_async() again. To determine
   * the socket readiness use the socket_readiness() function.
   *
   * @par Effects
   * Possible change of the returned value of communication_status().
   *
   * @par Exception safety guarantee
   * Basic.
   *
   * @remarks If called when `(communication_status() == Communication_status::failure)`,
   * it will dismiss all unhandled messages!
   *
   * @see connect(), communication_status(), socket_readiness().
   */
  DMITIGR_PGFE_API void connect_async();

  /**
   * @brief Attempts to connect to a PostgreSQL server.
   *
   * @param timeout - the value of `-1` means `options()->connect_timeout()`,
   * the value of `std::nullopt` means *eternity*.
   *
   * @par Effects
   * `(communication_status() == Communication_status::failure ||
   *    communication_status() == Communication_status::connected)`.
   *
   * @par Requires
   * `(!timeout || timeout->count() >= -1)`.
   *
   * @throws An instance of type Timed_out if the expression
   * `(connection_status() == Communication_status::connected)` will not
   * evaluates to `true` within the specified `timeout`.
   *
   * @par Exception safety guarantee
   * Basic.
   *
   * @see connect_async().
   */
  DMITIGR_PGFE_API void connect(std::optional<std::chrono::milliseconds> timeout = std::chrono::milliseconds{-1});

  /**
   * @brief Attempts to disconnect from a server.
   *
   * @par Effects
   * `(communication_status() == Communication_status::disconnected)`.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  void disconnect() noexcept
  {
    reset_session();
    conn_.reset(); // Discarding unhandled notifications btw.
    assert(communication_status() == Communication_status::disconnected);
    assert(is_invariant_ok());
  }

  /**
   * @brief Waits for readiness of the connection socket if it is unready.
   *
   * @returns The bit mask indicating the readiness of the connection socket.
   *
   * @param mask - the bit mask specifying the requested readiness of the
   * connection socket;
   *
   * @param timeout - the maximum amount of time to wait before return. The
   * value of `std::nullopt` denotes *eternity*.
   *
   * @par Requires
   * `((!timeout || timeout->count() >= -1) &&
   *    (communication_status() != Communication_status::failure) &&
   *    (communication_status() != Communication_status::disconnected))`.
   */
  DMITIGR_PGFE_API Socket_readiness wait_socket_readiness(Socket_readiness mask,
    std::optional<std::chrono::milliseconds> timeout = std::nullopt) const;

  /**
   * @brief Polls the readiness of the connection socket.
   *
   * @param mask - similar to wait_socket_readiness().
   *
   * @returns wait_socket_readiness(mask, std::chrono::milliseconds{});
   *
   * @see wait_socket_readiness().
   */
  DMITIGR_PGFE_API Socket_readiness socket_readiness(Socket_readiness mask) const;

  ///@}

  // ---------------------------------------------------------------------------

  /// @name Messages
  /// @{

  /**
   * @brief If input is available from the server, read it.
   *
   * This function should be called every time when the value returned by
   * collect_messages() is Response_status::unready and the socket is in
   * read-ready state.
   *
   * @see collect_messages(), socket_readiness().
   */
  void read_input()
  {
    if (!::PQconsumeInput(conn()))
      throw std::runtime_error{error_message()};
  }

  /**
   * @brief Collects and queue the messages of all kinds which was sent by the server.
   *
   * @returns The value of type Response_status.
   *
   * @param wait_response Indicates whether to wait for response (which assumes
   * the possible thread block).
   *
   * @par Requires
   * `is_connected()`.
   *
   * @par Effects
   * *Possible* signals and/or response are available.
   *
   * @par Exception safety guarantee
   * Basic.
   *
   * @see read_input().
   */
  DMITIGR_PGFE_API Response_status collect_messages(bool wait_response = false);

  /// @}

  // -----------------------------------------------------------------------------

  /**
   * @name Signals
   */
  /// @{

  /// @returns The released instance of type Notification if available.
  DMITIGR_PGFE_API Notification pop_notification() noexcept;

  /// An alias of a notice handler.
  using Notice_handler = std::function<void(const Notice&)>;

  /**
   * @brief Sets the handler for notices.
   *
   * By default, the notice handler just prints notices to the standard error
   * and never throws.
   *
   * @param handler - the handler to set.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @see handle_signals(), notice_handler().
   */
  void set_notice_handler(Notice_handler handler) noexcept
  {
    notice_handler_ = std::move(handler);
    assert(is_invariant_ok());
  }

  /// @returns The current notice handler.
  const Notice_handler& notice_handler() const noexcept
  {
    return notice_handler_;
  }

  /// An alias of a notification handler.
  using Notification_handler = std::function<void(Notification&&)>;

  /**
   * @brief Sets the handler for notifications.
   *
   * By default, a notification handler isn't set.
   *
   * @param handler - the handler to set.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @see handle_signals().
   */
  void set_notification_handler(Notification_handler handler) noexcept
  {
    notification_handler_ = std::move(handler);
    assert(is_invariant_ok());
  }

  /// @returns The current notification handler.
  const Notification_handler& notification_handler() const noexcept
  {
    return notification_handler_;
  }

  /**
   * @brief Call signals handlers.
   *
   * @par Exception safety guarantee
   * Basic.
   */
  void handle_signals()
  {
    if (!notifications_.empty()) {
      if (const auto& handle_notification = notification_handler()) {
        while (auto n = pop_notification())
          handle_notification(std::move(n));
      }
    }
  }

  ///@}

  // ---------------------------------------------------------------------------

  /// @name Responses
  /// @{

  /**
   * @returns `true` if some kind of the Response is awaited.
   *
   * @see wait_response().
   */
  bool is_awaiting_response() const noexcept
  {
    return !requests_.empty();
  }

  /**
   * @brief Waits a some kind of the Response if it is unavailable and awaited.
   *
   * @param timeout - the value of `-1` means `options()->wait_response_timeout()`,
   * the value of `std::nullopt` means *eternity*.
   *
   * @par Requires
   * `((!timeout || timeout->count() >= -1) && is_connected() && is_awaiting_response())`.
   *
   * @throws An instance of type Timed_out if the expression `response()`
   * will not evaluates to `true` within the specified `timeout`.
   *
   * @par Exception safety guarantee
   * Basic.
   *
   * @remarks All signals retrieved upon waiting the Response will be handled
   * by signals handlers being set.
   *
   * @see response().
   */
  DMITIGR_PGFE_API void wait_response(std::optional<std::chrono::milliseconds> timeout = std::chrono::milliseconds{-1});

  /**
   * @brief Similar to wait_response(), but throws Server_exception
   * if `(error() != std::nullopt)` after awaiting.
   */
  void wait_response_throw(std::optional<std::chrono::milliseconds> timeout = std::chrono::milliseconds{-1})
  {
    wait_response(timeout);
    throw_if_error();
  }

  /// @returns `true` if there is unhandled response available.
  bool has_response() const noexcept
  {
    return static_cast<bool>(response_);
  }

  /**
   * @brief Dismissing the last available Response.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @remarks It's more efficienlty than error(), wait_row() or wait_completion().
   */
  void dismiss_response() noexcept
  {
    response_.reset();
  }

  /**
   * @brief An alias of error handler.
   *
   * Being set, this handler is called when the server responded with an error.
   * If calling of this handler doesn't throw an exception and returns `false`
   * the instance of type Server_exception will be thrown eventually. If this
   * handler returns `true` then the error is considered handled and no further
   * action is taken.
   */
  using Error_handler = std::function<bool(std::shared_ptr<Error>)>;

  /**
   * @brief Sets the handler for custom errors.
   *
   * @param hander A handler to set.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @see Error_handler, error_handler().
   */
  void set_error_handler(Error_handler handler) noexcept
  {
    error_handler_ = std::move(handler);
    assert(is_invariant_ok());
  }

  /// @returns A current error handler.
  const Error_handler& error_handler() noexcept
  {
    return error_handler_;
  }

  /**
   * @returns The released instance of type Error if available.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @remarks Useful only if using async API.
   */
  Error error() noexcept
  {
    return response_ && (response_.status() == PGRES_FATAL_ERROR) ? Error{std::move(response_)} : Error{};
  }

  /**
   * @brief Waits for next row.
   *
   * @returns The awaited Row, or invalid instance.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @see dismiss_response(), wait_completion().
   */
  Row wait_row()
  {
    wait_response_throw();
    return response_ && (response_.status() == PGRES_SINGLE_TUPLE) ? Row{std::move(response_), shared_field_names_} : Row{};
  }

  /**
   * @returns {wait_row(), wait_completion()}.
   *
   * @see wait_row(), wait_completion()
   */
  std::pair<Row, Completion> wait_row_then_completion()
  {
    auto row = wait_row();
    auto comp = wait_completion();
    return {std::move(row), std::move(comp)};
  }

  /**
   * @return Waits for next Row and discards the rows followed after that returned row.
   *
   * @par Effects
   * Completion response are available (if not Error generated).
   *
   * @par Exception safety guarantee
   * Strong
   *
   * @see wait_row(), dismiss_response().
   */
  Row wait_row_then_discard()
  {
    auto row = wait_row();
    wait_completion();
    return row;
  }

  /**
   * @brief Waits for Completion and throws Server_expection on Error. Skips the rows (if any).
   *
   * @returns The awaited Completion, or invalid instance.
   *
   * @param timeout - the value of `-1` means `options()->wait_completion_timeout()`,
   * the value of `std::nullopt` means *eternity*.
   *
   * @throws An instance of type Timed_out if the expression `is_awaiting_response()`
   * will not evaluates to `false` within the specified `timeout`.
   *
   * @par Requires
   * `((!timeout || timeout->count() >= -1) && is_connected() && is_awaiting_response())`.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @remarks There is no necessity to handle Completion explicitly. It will be
   * dismissed automatically when appropriate.
   */
  DMITIGR_PGFE_API Completion wait_completion(std::optional<std::chrono::milliseconds> timeout = std::chrono::milliseconds{-1});

  /**
   * @returns The pointer to the instance of type Prepared_statement if available.
   *
   * @remarks The object pointed by the returned value is owned by this instance.
   */
  Prepared_statement* prepared_statement() const noexcept
  {
    Prepared_statement* result{};
    if (response_ && (response_.status() == PGRES_COMMAND_OK) && (response_request_id_ == Request_id::prepare_statement)) {
      result = register_ps(std::move(request_prepared_statement_));
      assert(!request_prepared_statement_);
      response_.reset();
    }
    return result;
  }

  /**
   * @returns The prepared statement by its name, or `nullptr` if prepared
   * statement with the given name is unknown by Connection. (This is possible
   * when the statement is prepared by using the SQL command `PREPARE`. Such a
   * statement must be described before using this method.)
   *
   * @param name - the name of the prepared statement.
   *
   * @remarks The object pointed by the returned value is owned by this instance.
   *
   * @see describe_prepared_statement(), describe_prepared_statement_async().
   */
  Prepared_statement* prepared_statement(const std::string& name) const noexcept
  {
    return ps(name);
  }

  ///@}

  // ---------------------------------------------------------------------------

  /// @name Requests
  /// @{

  /**
   * @returns `true` if the connection is ready for requesting a server in a
   * non-blocking manner.
   *
   * @see is_ready_for_request().
   */
  bool is_ready_for_async_request() const noexcept
  {
    return is_connected() && requests_.empty() && (!response_ || [s = response_.status()]
    {
      return s == PGRES_TUPLES_OK || s == PGRES_COMMAND_OK || s == PGRES_EMPTY_QUERY || s == PGRES_BAD_RESPONSE;
    }());
  }

  /**
   * @returns `true` if the connection is ready for requesting a server.
   *
   * @see is_awaiting_response().
   */
  bool is_ready_for_request() const noexcept
  {
    // Same as is_ready_for_async_request() at the moment.
    return is_ready_for_async_request();
  }

  /**
   * @brief Submits the SQL query(-es) to a server.
   *
   * @par Awaited responses
   *   - if the query provokes an error: Error;
   *   - if the query does not provokes producing rows: Completion;
   *   - if the query provokes producing rows: the set of Row (if any), and
   *     finally the Completion.
   *
   * @param queries - the string, containing the SQL query(-es). Adjacent
   * queries must be separated by a semicolon.
   *
   * @par Effects
   * `is_awaiting_response()`.
   *
   * @par Requires
   * `is_ready_for_async_request()`.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @remarks All queries specified in `queries` are executed in a transaction.
   * Each query will provoke producing the separate flow of responses. Therefore,
   * if one of them provokes an Error, then the transaction will be aborted and
   * the queries which were not yet executed will be rejected.
   *
   * @see prepare_statement_async().
   */
  DMITIGR_PGFE_API void perform_async(const std::string& queries);

  /**
   * @brief Similar to perform_async(), but waits for the first Response and
   * throws Server_exception if awaited Response is an Error.
   *
   * @par Requires
   * `is_ready_for_request()`.
   *
   * @par Exception safety guarantee
   * Basic.
   */
  void perform(const std::string& queries)
  {
    assert(is_ready_for_request());
    perform_async(queries);
    wait_response_throw();
  }

  /**
   * @brief Submits a request to a server to prepare the statement.
   *
   * @par Awaited responses
   * Prepared_statement
   *
   * @param statement - the preparsed SQL string;
   * @param name - the name of the statement to be prepared.
   *
   * @par Effects
   * - `is_awaiting_response()` - just after the successful request submission;
   * - `(prepared_statement(name) != nullptr && prepared_statement(name)->is_preparsed())` - just
   * after the successful response collection.
   *
   * @par Requires
   * `(statement && !statement->has_missing_parameters() && is_ready_for_async_request())`.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @remarks It is recommended to specify the types of the parameters by
   * using explicit type casts to avoid ambiguities or type mismatch mistakes.
   * For example:
   *   @code{sql}
   *     -- Force to use generate_series(int, int) overload.
   *     SELECT generate_series($1::int, $2::int);
   *   @endcode
   * This forces parameters `$1` and `$2` to be treated as of type `integer`
   * and thus the corresponding overload will be used in this case.
   *
   * @see unprepare_statement_async().
   */
  void prepare_statement_async(const Sql_string& statement, const std::string& name = {})
  {
    assert(!statement.has_missing_parameters());
    prepare_statement_async__(statement.to_query_string().c_str(), name.c_str(), &statement); // can throw
  }

  /**
   * @brief Same as prepare_statement_async() except the statement will be send
   * as-is, i.e. without preparsing.
   */
  void prepare_statement_async_as_is(const std::string& statement, const std::string& name = {})
  {
    prepare_statement_async__(statement.c_str(), name.c_str(), nullptr); // can throw
  }

  /**
   * @returns `(prepare_statement_async(), wait_response_throw(), prepared_statement())`
   *
   * @par Requires
   * `(statement && !statement->has_missing_parameters() && is_ready_for_request())`.
   *
   * @par Exception safety guarantee
   * Basic.
   *
   * @remarks See remarks of prepare_statement_async().
   *
   * @see unprepare_statement().
   */
  Prepared_statement* prepare_statement(const Sql_string& statement, const std::string& name = {})
  {
    using M = void(Connection::*)(const Sql_string&, const std::string&);
    return prepare_statement__(static_cast<M>(&Connection::prepare_statement_async), statement, name);
  }

  /**
   * @brief Same as prepare_statement() except the statement will be send as-is,
   * i.e. without preparsing.
   */
  Prepared_statement* prepare_statement_as_is(const std::string& statement, const std::string& name = {})
  {
    return prepare_statement__(&Connection::prepare_statement_async_as_is, statement, name);
  }

  /**
   * @brief Submits a request to a PostgreSQL server to describe
   * the prepared statement.
   *
   * @par Awaiting responses
   * Prepared_statement
   *
   * @param name - the name of the prepared statement.
   *
   * @par Effects
   * - `is_awaiting_response()` - just after the successful request submission;
   * - `(prepared_statement(name) != nullptr && prepared_statement(name)->is_described())` - just
   * after the successful response collection.
   *
   * @par Requires
   * `is_ready_for_async_request()`.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @see describe_prepared_statement().
   */
  DMITIGR_PGFE_API void describe_prepared_statement_async(const std::string& name);

  /**
   * @returns `(describe_prepared_statement_async(), wait_response_throw(), prepared_statement())`
   *
   * @par Requires
   * `is_ready_for_request()`.
   *
   * @par Exception safety guarantee
   * Basic.
   *
   * @see unprepare_statement().
   */
  Prepared_statement* describe_prepared_statement(const std::string& name)
  {
    assert(is_ready_for_request());
    describe_prepared_statement_async(name);
    wait_response_throw();
    auto* p = ps(*request_prepared_statement_name_);
    if (!p)
      p = register_ps(Prepared_statement{std::move(*request_prepared_statement_name_),
                                         this, static_cast<std::size_t>(response_.field_count())});
    p->set_description(std::move(response_));
    request_prepared_statement_name_.reset();
    return p;
  }

  /**
   * @brief Submits a request to a PostgreSQL server to close
   * the prepared statement.
   *
   * @par Awaited responses
   * Completion
   *
   * @param name - the name of the prepared statement.
   *
   * @par Effects
   * - `is_awaiting_response()` - just after the successful request submission;
   * - `(prepared_statement(name) == nullptr)` - just after the successful response collection.
   *
   * @par Requires
   * `(is_ready_for_async_request() && !name.empty())`.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @remarks It's impossible to unprepare an unnamed prepared statement
   * at the moment.
   *
   * @see unprepare_statement().
   */
  DMITIGR_PGFE_API void unprepare_statement_async(const std::string& name);

  /**
   * @returns `(unprepare_statement_async(const std::string& name), wait_response_throw())`
   *
   * @par Requires
   * `is_ready_for_request()`.
   *
   * @par Exception safety guarantee
   * Basic.
   */
  void unprepare_statement(const std::string& name)
  {
    assert(is_ready_for_request());
    unprepare_statement_async(name);
    wait_response_throw(); // Checking invariant.
  }

  /**
   * @brief Submits the requests to a server to prepare and execute the unnamed
   * statement from the preparsed SQL string, and waits for a response.
   *
   * @par Awaited responses
   * Similar to perform_async().
   *
   * @param statement - the *preparsed* statement to execute;
   * @param parameters - the parameters to bind with the parameterized statement.
   *
   * @par Requires
   * `(statement && !statement->has_missing_parameters() && is_ready_for_request())`.
   *
   * @par Exception safety guarantee
   * Basic.
   *
   * @remarks See remarks of prepare_statement().
   */
  template<typename ... Types>
  void execute(const Sql_string& statement, Types&& ... parameters)
  {
    auto* const ps = prepare_statement(statement);
    ps->set_parameters(std::forward<Types>(parameters)...);
    ps->execute();
  }

  /**
   * @brief Submits the requests to a server to invoke the specified function
   * and waits for a response.
   *
   * If `function` returns table with multiple columns or has multiple output
   * parameters they are can be accessed as usual by using Row::data().
   *
   * If `function` have named parameters it can be called using either
   * positional, named or mixed notation.
   *
   * When using positional notation all arguments specified traditionally in
   * order, for example:
   * @code
   * conn->invoke("generate_series", 1, 3);
   * @endcode
   *
   * When using named notation, each argument is specified using object of type
   * Named_argument (or it alias - _), for example:
   *
   * @code
   * conn->invoke("person_info", _{"name", "Christopher"}, _{"id", 1});
   * @endcode
   *
   * When using mixed notation which combines positional and named notation,
   * named arguments cannot precede positional arguments. The compile time check
   * will be performed to enforce that. For example:
   *
   * @code
   * conn->invoke("person_info", 1, _{"name", "Christopher"});
   * @endcode
   *
   * See <a href="https://www.postgresql.org/docs/current/static/sql-syntax-calling-funcs.html">calling functions</a>
   * section of the PostgreSQL documentation for the full details on calling
   * notations.
   *
   * @par Awaited responses
   * Similar to execute().
   *
   * @param function - the function name to invoke;
   * @param arguments - the function arguments.
   *
   * @par Requires
   * `(!function.empty() && is_ready_for_request())`.
   *
   * @par Exception safety guarantee
   * Basic.
   *
   * @remarks It may be problematic to invoke overloaded functions with same
   * number of parameters. A SQL query with explicit type casts should be
   * executed is such a case. See remarks of prepare_statement_async().
   *
   * @see invoke_unexpanded(), call(), execute().
   */
  template<typename ... Types>
  void invoke(std::string_view function, Types&& ... arguments)
  {
    static_assert(is_routine_arguments_ok__<Types...>(), "named arguments cannot precede positional arguments");
    const auto stmt = routine_query__(function, "SELECT * FROM", std::forward<Types>(arguments)...);
    execute(stmt, std::forward<Types>(arguments)...);
  }

  /**
   * @brief Similar to invoke() but even if `function` returns table with
   * multiple columns or has multiple output parameters the result row is
   * always consists of exactly one field.
   *
   * @remarks This method is for specific use and in most cases invoke()
   * should be used instead.
   *
   * @see invoke(), call(), execute().
   */
  template<typename ... Types>
  void invoke_unexpanded(std::string_view function, Types&& ... arguments)
  {
    static_assert(is_routine_arguments_ok__<Types...>(), "named arguments cannot precede positional arguments");
    const auto stmt = routine_query__(function, "SELECT", std::forward<Types>(arguments)...);
    execute(stmt, std::forward<Types>(arguments)...);
  }

  /**
   * @brief Submits the requests to a server to invoke the specified procedure
   * and waits for a response.
   *
   * This method is similar to invoke(), but for procedures rather than functions.
   *
   * @remarks PostgreSQL supports procedures since version 11.
   *
   * @see invoke(), call(), execute().
   */
  template<typename ... Types>
  void call(std::string_view procedure, Types&& ... arguments)
  {
    static_assert(is_routine_arguments_ok__<Types...>(), "named arguments cannot precede positional arguments");
    const auto stmt = routine_query__(procedure, "CALL", std::forward<Types>(arguments)...);
    execute(stmt, std::forward<Types>(arguments)...);
  }

  /**
   * @brief Sets the default data format of the result for a next prepared
   * statement execution.
   *
   * @param format - the data format to set.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  void set_result_format(const Data_format format) noexcept
  {
    default_result_format_ = format;
    assert(is_invariant_ok());
  }

  /**
   * @returns The default data format of the result for a next prepared
   * statement execution.
   */
  Data_format result_format() const noexcept
  {
    return default_result_format_;
  }

  ///@}

  // ---------------------------------------------------------------------------

  /// @name Large objects
  /// @{

  /**
   * @brief Submits a request to create the large object and waits the result.
   *
   * @param oid A desired oid. If `invalid_oid` assumes unused oid.
   *
   * @returns A valid oid if successful, or `invalid_oid` otherwise.
   *
   * @par Requires
   * `(is_ready_for_request())`.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  DMITIGR_PGFE_API Oid create_large_object(Oid oid = invalid_oid) noexcept;

  /**
   * @brief Submits a request to open the large object and waits the result.
   *
   * @returns A valid instance if successful, or invalid instance otherwise.
   *
   * @par Requires
   * `(is_ready_for_request())`.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  DMITIGR_PGFE_API Large_object open_large_object(Oid oid, Large_object_open_mode mode) noexcept;

  /**
   * @brief Submits a request to remove the large object and waits the result.
   *
   * @returns `true` on success.
   *
   * @par Requires
   * `(is_ready_for_request())`.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  bool remove_large_object(Oid oid) noexcept
  {
    assert(is_ready_for_request());
    return ::lo_unlink(conn(), oid);
  }

  /**
   * @brief Submits multiple requests to import the specified file as a large
   * object.
   *
   * @returns The OID of the new large object on success, or `invalid_oid`
   * otherwise.
   *
   * @par Requires
   * `(is_ready_for_request())`.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  Oid import_large_object(const std::filesystem::path& filename, Oid oid = invalid_oid) noexcept
  {
    assert(is_ready_for_request());
    return ::lo_import_with_oid(conn(), filename.c_str(), oid);
  }

  /**
   * @brief Submits multiple requests to export the specified large object
   * to the specified file.
   *
   * @returns `true` on success.
   *
   * @par Requires
   * `(is_ready_for_request())`.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  bool export_large_object(Oid oid, const std::filesystem::path& filename) noexcept
  {
    assert(is_ready_for_request());
    return ::lo_export(conn(), oid, filename.c_str()) == 1; // lo_export returns -1 on failure
  }

  /// @}

  // ---------------------------------------------------------------------------

  /// @name Utilities
  /// @{

  /**
   * @brief Retrieves all the rows of the connection and converts them into
   * objects of specified type.
   *
   * The conversion is performed by applying the conversion routine
   * `Conversions<typename Container::value_type>::to_type()` to each row.
   *
   * @returns The container of objects retrieved from the connection.
   *
   * @par Exception safety guarantee
   * Basic.
   */
  template<class Container = std::vector<Row>>
  Container wait_rows()
  {
    Row_collector<Container> result;
    while (auto&& r = wait_row())
      result.collect(std::move(r));
    return result.container;
  }

  /**
   * @brief Quotes the given string to be used as a literal in a SQL query.
   *
   * @returns The suitably quoted literal.
   *
   * @param literal - the literal to quote.
   *
   * @par Requires
   * `is_connected()`.
   *
   * @remarks Using of parameterized prepared statement should be considered as
   * the better alternative comparing to including the quoted data into a query.
   *
   * @remarks Note, the result is depends on session properties (such as a
   * character encoding). Therefore using it in queries which are submitted
   * by using connections with other session properties is not correct.
   *
   * @see Prepared_statement.
   */
  DMITIGR_PGFE_API std::string to_quoted_literal(const std::string& literal) const;

  /**
   * @brief Quotes the given string to be used as an identifier in a SQL query.
   *
   * @param identifier - the identifier to quote.
   *
   * @returns The suitably quoted identifier.
   *
   * @par Requires
   * `is_connected()`.
   *
   * @remarks Note, the result is depends on session properties (such as a
   * character encoding). Therefore using it in queries which are submitted
   * by using connections with other session properties is not correct.
   *
   * @see Prepared_statement.
   */
  DMITIGR_PGFE_API std::string to_quoted_identifier(const std::string& identifier) const;

  /**
   * @brief Encodes the binary data into the textual representation to be used
   * in a SQL query.
   *
   * @param binary_data - the Data of the binary format to escape.
   *
   * @returns The encoded data in the hex format.
   *
   * @par Requires
   * `(is_connected() && binary_data && (binary_data->format() == Data_format::binary))`.
   *
   * @remarks Using of parameterized prepared statement should be considered as
   * the better alternative comparing to including the encoded binary data into
   * a query.
   *
   * @remarks Note, the result is depends on session properties (such as a
   * character encoding). Therefore using it in queries which are submitted
   * by using connections with other session properties is not correct.
   *
   * @see Prepared_statement.
   */
  std::unique_ptr<Data> to_hex_data(const Data* binary_data) const
  {
    auto [storage, size] = to_hex_storage(binary_data);
    return Data::make(std::move(storage), size, Data_format::text);
  }

  /**
   * @brief Similar to to_hex_data(const Data*).
   *
   * @returns The encoded string in the hex format.
   *
   * @see to_hex_data().
   */
  std::string to_hex_string(const Data* binary_data) const
  {
    const auto [storage, size] = to_hex_storage(binary_data);
    return std::string{reinterpret_cast<const char*>(storage.get()), size};
  }

  ///@}
private:
  friend Large_object;
  friend Prepared_statement;

  // ---------------------------------------------------------------------------
  // Persistent data
  // ---------------------------------------------------------------------------

  // Persistent data / constant data
  Connection_options options_;

  // Persistent data / public-modifiable data
  Error_handler error_handler_;
  Notice_handler notice_handler_;
  Notification_handler notification_handler_;
  Data_format default_result_format_{Data_format::text};

  // Persistent data / private-modifiable data
  std::unique_ptr< ::PGconn> conn_;
  std::optional<Communication_status> polling_status_;
  ::PGconn* conn() const noexcept { return conn_.get(); }

  // ---------------------------------------------------------------------------
  // Session data / requests data
  // ---------------------------------------------------------------------------

  enum class Request_id {
    perform = 1,
    execute,
    prepare_statement,
    describe_prepared_statement,
    unprepare_statement
  };

  std::optional<std::chrono::system_clock::time_point> session_start_time_;

  mutable std::queue<Notification> notifications_;

  mutable detail::pq::Result response_;
  detail::pq::Result pending_response_;
  mutable std::optional<Transaction_block_status> transaction_block_status_;
  mutable std::optional<std::int_fast32_t> server_pid_;
  mutable std::list<Prepared_statement> named_prepared_statements_;
  mutable Prepared_statement unnamed_prepared_statement_;
  std::shared_ptr<std::vector<std::string>> shared_field_names_;

  Request_id response_request_id_;
  std::queue<Request_id> requests_; // for now only 1 request can be queued
  mutable Prepared_statement request_prepared_statement_;
  std::optional<std::string> request_prepared_statement_name_;

  bool is_invariant_ok() const noexcept;

  // ---------------------------------------------------------------------------
  // Session data helpers
  // ---------------------------------------------------------------------------

  void reset_session() noexcept;

  // ---------------------------------------------------------------------------
  // Handlers
  // ---------------------------------------------------------------------------

  static void notice_receiver(void* const arg, const ::PGresult* const r) noexcept;
  static void default_notice_handler(const Notice& n) noexcept;

  // ---------------------------------------------------------------------------
  // Prepared statement helpers
  // ---------------------------------------------------------------------------

  void prepare_statement_async__(const char* const query, const char* const name, const Sql_string* const preparsed);

  template<typename M, typename T>
  Prepared_statement* prepare_statement__(M&& prepare, T&& statement, const std::string& name)
  {
    assert(is_ready_for_request());
    (this->*prepare)(std::forward<T>(statement), name);
    wait_response_throw();
    return prepared_statement();
  }

  /*
   * Attempts to find the prepared statement.
   *
   * @returns The pointer to the founded prepared statement, or `nullptr` if not found.
   */
  Prepared_statement* ps(const std::string& name) const noexcept;

  /*
   * Register prepared statement.
   *
   * @returns The pointer to the registered prepared statement.
   */
  Prepared_statement* register_ps(Prepared_statement&& ps) const noexcept;

  // Unregisters the prepared statement.
  void unregister_ps(const std::string& name) noexcept;

  // ---------------------------------------------------------------------------
  // Utilities helpers
  // ---------------------------------------------------------------------------

  int socket() const noexcept
  {
    return ::PQsocket(conn());
  }

  void throw_if_error();

  std::string error_message() const;

  bool is_out_of_memory() const
  {
    constexpr char msg[] = "out of memory";
    return !std::strncmp(::PQerrorMessage(conn()), msg, sizeof(msg) - 1);
  }

  std::pair<std::unique_ptr<void, void(*)(void*)>, std::size_t> to_hex_storage(const pgfe::Data* const binary_data) const;

  // ---------------------------------------------------------------------------
  // Large Object private API
  // ---------------------------------------------------------------------------

  DMITIGR_PGFE_API bool close(Large_object& lo) noexcept;
  DMITIGR_PGFE_API std::int_fast64_t seek(Large_object& lo, std::int_fast64_t offset, Large_object_seek_whence whence) noexcept;
  DMITIGR_PGFE_API std::int_fast64_t tell(Large_object& lo) noexcept;
  DMITIGR_PGFE_API bool truncate(Large_object& lo, const std::int_fast64_t new_size) noexcept;
  DMITIGR_PGFE_API int read(Large_object& lo, char* const buf, const std::size_t size) noexcept;
  DMITIGR_PGFE_API int write(Large_object& lo, const char* const buf, const std::size_t size) noexcept;

  // ---------------------------------------------------------------------------
  // call/invoke helpers
  // ---------------------------------------------------------------------------

  template<typename ... Types>
  std::string routine_query__(std::string_view function, std::string_view invocation, Types&& ... arguments)
  {
    assert(!function.empty());
    assert(invocation == "SELECT * FROM" || invocation == "SELECT" || invocation == "CALL");
    std::string result;
    if constexpr (sizeof...(arguments) > 0) {
      result.reserve(64);
      result.append(invocation).append(" ");
      result.append(function).append("(");
      result.append(routine_arguments__(std::make_index_sequence<sizeof ... (Types)>{}, std::forward<Types>(arguments)...));
      result.append(")");
    } else {
      result.reserve(14 + function.size() + 2);
      result.append(invocation).append(" ").append(function).append("()");
    }
    return result;
  }

  template<std::size_t ... I, typename ... Types>
  std::string routine_arguments__(std::index_sequence<I...>, Types&& ... arguments)
  {
    static_assert(sizeof...(arguments) > 0);
    static_assert(sizeof...(arguments) == sizeof...(I));
    std::string result;
    (result.append(routine_argument__(arguments, I)).append(","), ...);
    result.pop_back();
    return result;
  }

  template<typename T>
  std::string routine_argument__(const T&, const std::size_t i)
  {
    return std::string{"$"}.append(std::to_string(i + 1));
  }

  std::string routine_argument__(const Named_argument& na, const std::size_t)
  {
    return std::string{na.name()}.append("=>:").append(na.name());
  }

  template<typename T = void>
  static constexpr bool is_routine_arguments_ok__()
  {
    return true;
  }

  template<typename T1, typename T2, typename ... Types>
  static constexpr bool is_routine_arguments_ok__()
  {
    using U1 = std::decay_t<T1>;
    using U2 = std::decay_t<T2>;
    constexpr bool is_named_1 = std::is_same_v<U1, Named_argument>;
    constexpr bool is_named_2 = std::is_same_v<U2, Named_argument>;
    constexpr bool is_both_positionals = !is_named_1 && !is_named_2;
    constexpr bool is_both_named = is_named_1 && is_named_2;
    constexpr bool is_named_follows_positional = !is_named_1 && is_named_2;
    constexpr bool is_ok = (is_both_positionals || is_both_named || is_named_follows_positional);
    return is_ok && is_routine_arguments_ok__<T2, Types...>();
  }
};

} // namespace dmitigr::pgfe

#ifdef DMITIGR_PGFE_HEADER_ONLY
#include "dmitigr/pgfe/connection.cpp"
#include "dmitigr/pgfe/prepared_statement.cpp"
#endif

#endif  // DMITIGR_PGFE_CONNECTION_HPP
