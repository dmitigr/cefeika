// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or wsbe.hpp

#include "dmitigr/wsbe/listener.hpp"
#include "dmitigr/wsbe/listener_options.hpp"
#include "dmitigr/wsbe/server_connection.hpp"
#include "dmitigr/wsbe/implementation_header.hpp"

#include "dmitigr/util/debug.hpp"

#include <uwebsockets/App.h>

#include <vector>

namespace dmitigr::wsbe::detail {

class iListener final {
public:
  using Options = Listener_options;

  explicit iListener(Options options)
    : options_{std::move(options)}
  {}

  const Options& options() const
  {
    return options_;
  }

  bool is_listening() const
  {
    return listening_socket_;
  }

  void listen()
  {
    throw 1;
  }

  void close()
  {
    throw 1;
  }

private:
  Listener_options options_;
  std::vector<std::shared_ptr<Server_connection>> connections_;
  us_listen_socket_t* listening_socket_{};
};

} // namespace dmitigr::wsbe::detail

namespace dmitigr::wsbe {

DMITIGR_WSBE_INLINE Listener::Listener(Options options)
  : rep_{std::make_unique<detail::iListener>(std::move(options))}
{
  DMITIGR_ASSERT(rep_);
}

DMITIGR_WSBE_INLINE auto Listener::options() const -> const Options&
{
  return rep_->options();
}

DMITIGR_WSBE_INLINE bool Listener::is_listening() const
{
  return rep_->is_listening();
}

DMITIGR_WSBE_INLINE void Listener::listen()
{
  rep_->listen();
}

DMITIGR_WSBE_INLINE void Listener::close()
{
  rep_->close();
}

} // namespace dmitigr::wsbe

#include "dmitigr/wsbe/implementation_footer.hpp"
