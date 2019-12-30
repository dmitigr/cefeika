// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or wsbe.hpp

#ifndef DMITIGR_WSBE_LISTENER_HPP
#define DMITIGR_WSBE_LISTENER_HPP

#include "dmitigr/wsbe/basics.hpp"
#include "dmitigr/wsbe/dll.hpp"
#include "dmitigr/wsbe/types_fwd.hpp"

#include <memory>

namespace dmitigr::wsbe {

/**
 * @brief A WebSockets listener.
 */
class Listener {
public:
  /// @brief Alias of Listener_options.
  using Options = Listener_options;

  /**
   * @brief The destructor.
   */
  virtual ~Listener() = default;

  /// @name Constructors
  /// @{

  /**
   * @brief Constructs an instance of Listener.
   */
  explicit DMITIGR_WSBE_API Listener(Options options);

  /// @}

  /**
   * @returns Options of the listener.
   */
  virtual DMITIGR_WSBE_API const Options& options() const;

  /**
   * @returns `true` if the listener is listening for new client connections,
   * or `false` otherwise.
   */
  DMITIGR_WSBE_API bool is_listening() const;

  /**
   * @brief Starts listening.
   *
   * @par Requires
   * `!is_listening()`.
   */
  DMITIGR_WSBE_API void listen();

  /**
   * @brief Stops listening.
   */
  DMITIGR_WSBE_API void close();

private:
  /**
   * @returns The new connection instance.
   *
   * @brief This function to be called on every accepted connection in
   * order to create an instance of class derived from Server_connection.
   */
  virtual std::shared_ptr<Server_connection> make_connection() const = 0;

private:
  std::unique_ptr<detail::iListener> rep_;
};

} // namespace dmitigr::wsbe

#ifdef DMITIGR_WSBE_HEADER_ONLY
#include "dmitigr/wsbe/listener.cpp"
#endif

#endif  // DMITIGR_WSBE_LISTENER_HPP
