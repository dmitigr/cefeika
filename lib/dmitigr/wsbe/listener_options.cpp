// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or wsbe.hpp

#include "dmitigr/wsbe/listener_options.hpp"
#include "dmitigr/wsbe/implementation_header.hpp"

#include <dmitigr/util/debug.hpp>
#include <dmitigr/util/net.hpp>

namespace dmitigr::wsbe::detail {

/**
 * @brief The representation of Listener_options.
 */
class iListener_options final {
public:
  explicit iListener_options(std::unique_ptr<net::Listener_options>&& options)
    : options_{std::move(options)}
  {
    DMITIGR_ASSERT(is_invariant_ok());
  }

  iListener_options(std::string address, const int port, const int backlog)
    : iListener_options{net::Listener_options::make(std::move(address), port, backlog)}
  {}

  const net::Endpoint_id* endpoint_id() const
  {
    return options_->endpoint_id();
  }

private:
  friend iListener;
  friend Listener_options;

  std::unique_ptr<net::Listener_options> options_;

  bool is_invariant_ok() const
  {
    return static_cast<bool>(options_);
  }
};

} // namespace dmitigr::wsbe::detail

namespace dmitigr::wsbe {

DMITIGR_WSBE_INLINE Listener_options::~Listener_options() = default;

DMITIGR_WSBE_INLINE Listener_options::Listener_options(std::string address, const int port)
  : rep_{std::make_unique<detail::iListener_options>(std::move(address), port, /*FIXME: backlog*/ 512)}
{
  DMITIGR_ASSERT(static_cast<bool>(rep_));
}

DMITIGR_WSBE_INLINE Listener_options::Listener_options(const Listener_options& rhs)
  : rep_{std::make_unique<detail::iListener_options>(rhs.rep_->options_->to_listener_options())}
{
  DMITIGR_ASSERT(static_cast<bool>(rep_));
}

DMITIGR_WSBE_INLINE Listener_options& Listener_options::operator=(const Listener_options& rhs)
{
  if (this != &rhs) {
    rep_->options_ = rhs.rep_->options_->to_listener_options();
    DMITIGR_ASSERT(static_cast<bool>(rep_));
  }
  return *this;
}

DMITIGR_WSBE_INLINE const net::Endpoint_id* Listener_options::endpoint_id() const
{
  return rep_->endpoint_id();
}

} // namespace dmitigr::wsbe

#include "dmitigr/wsbe/implementation_footer.hpp"
