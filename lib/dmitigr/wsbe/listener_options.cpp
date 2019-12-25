// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or wsbe.hpp

#include "dmitigr/wsbe/listener_options.hpp"
#include "dmitigr/wsbe/implementation_header.hpp"

#include <dmitigr/util/debug.hpp>
#include <dmitigr/util/net.hpp>

#include <stdexcept>

namespace dmitigr::wsbe::detail {

/**
 * @brief The implementation of Listener_options.
 */
class iListener_options final : public Listener_options {
public:
  /**
   * @brief See Listener_options::make().
   */
  explicit iListener_options(std::unique_ptr<net::Listener_options> options)
    : options_{std::move(options)}
  {
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @brief See Listener_options::make().
   */
  iListener_options(std::string address, const int port, const int backlog)
    : iListener_options{net::Listener_options::make(std::move(address), port, backlog)}
  {}

  std::unique_ptr<Listener> make_listener() const override; // defined in listener.cpp

  std::unique_ptr<Listener_options> to_listener_options() const override
  {
    return std::make_unique<iListener_options>(options_->to_listener_options());
  }

  const net::Endpoint_id* endpoint_id() const  override
  {
    return options_->endpoint_id();
  }

private:
  friend iListener;

  std::unique_ptr<net::Listener_options> options_;

  bool is_invariant_ok() const
  {
    return static_cast<bool>(options_);
  }

  iListener_options() = default;
};

} // namespace dmitigr::wsbe::detail

namespace dmitigr::wsbe {

DMITIGR_WSBE_INLINE std::unique_ptr<Listener_options>
Listener_options::make(std::string address, const int port)
{
  using detail::iListener_options;
  return std::make_unique<iListener_options>(std::move(address), port, /*backlog*/ 512);
}

} // namespace dmitigr::wsbe

#include "dmitigr/wsbe/implementation_footer.hpp"
