// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include "dmitigr/ws/basics.hpp"
#include "dmitigr/ws/listener_options.hpp"
#include "dmitigr/ws/implementation_header.hpp"

#include "dmitigr/net.hpp"
#include "dmitigr/util/debug.hpp"

#include <limits>
#include <stdexcept>

namespace dmitigr::ws::detail {

inline namespace validators {

inline void validate(const bool condition, const std::string& option_name)
{
  if (!condition)
    throw std::logic_error{"invalid value of \"" + option_name + "\" connection option"};
}

} // inline namespace validators

/**
 * @brief The representation of Listener_options.
 */
class iListener_options final {
public:
  iListener_options(const iListener_options& rhs)
    : net_options_{rhs.net_options_->to_listener_options()}
    , idle_timeout_{rhs.idle_timeout_}
    , max_payload_size_{rhs.max_payload_size_}
  {
    DMITIGR_ASSERT(is_invariant_ok());
  }

  iListener_options& operator=(const iListener_options& rhs)
  {
    iListener_options rhs_copy{rhs};
    swap(rhs_copy);
    DMITIGR_ASSERT(is_invariant_ok());
    return *this;
  }

  iListener_options(iListener_options&& rhs) = default;
  iListener_options& operator=(iListener_options&& rhs) = default;

  iListener_options(std::string address, const int port, const int backlog)
    : net_options_{net::Listener_options::make(std::move(address), port, backlog)}
  {
    DMITIGR_ASSERT(is_invariant_ok());
  }

  const net::Endpoint_id* endpoint_id() const
  {
    return net_options_->endpoint_id();
  }

  void set_idle_timeout(std::optional<std::chrono::milliseconds> value)
  {
    if (value)
      validate(value->count() >= 0, "connect timeout");
    idle_timeout_ = std::move(value);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  std::optional<std::chrono::milliseconds> idle_timeout() const
  {
    return idle_timeout_;
  }

  void set_max_payload_size(const std::size_t value)
  {
    validate(value <= std::numeric_limits<int>::max(), "max payload size");
    max_payload_size_ = value;
    DMITIGR_ASSERT(is_invariant_ok());
  }

  std::size_t max_payload_size() const
  {
    return max_payload_size_;
  }

  void swap(iListener_options& other)
  {
    net_options_.swap(other.net_options_);
    idle_timeout_.swap(other.idle_timeout_);
    std::swap(max_payload_size_, other.max_payload_size_);
  }

private:
  friend iListener;
  friend Listener_options;

  std::unique_ptr<net::Listener_options> net_options_;
  std::optional<std::chrono::milliseconds> idle_timeout_;
  std::size_t max_payload_size_{static_cast<std::size_t>(std::numeric_limits<int>::max())};

  bool is_invariant_ok() const
  {
    return net_options_ && (net_options_->endpoint_id()->communication_mode() == net::Communication_mode::net) &&
      (!idle_timeout_ || idle_timeout_->count() >= 0) &&
      (max_payload_size_ <= std::numeric_limits<int>::max());
  }
};

} // namespace dmitigr::ws::detail

namespace dmitigr::ws {

DMITIGR_WS_INLINE Listener_options::~Listener_options() = default;

DMITIGR_WS_INLINE Listener_options::Listener_options(std::string address, const int port)
  : rep_{std::make_unique<detail::iListener_options>(std::move(address), port, /*FIXME: backlog*/ 512)}
{
  DMITIGR_ASSERT(rep_ && rep_->is_invariant_ok());
}

DMITIGR_WS_INLINE Listener_options::Listener_options(const Listener_options& rhs)
  : rep_{std::make_unique<detail::iListener_options>(*rhs.rep_)}
{
  DMITIGR_ASSERT(rep_ && rep_->is_invariant_ok());
}

DMITIGR_WS_INLINE Listener_options& Listener_options::operator=(const Listener_options& rhs)
{
  Listener_options rhs_copy{rhs};
  swap(rhs_copy);
  DMITIGR_ASSERT(rep_ && rep_->is_invariant_ok());
  return *this;
}

DMITIGR_WS_INLINE Listener_options::Listener_options(Listener_options&& rhs) = default;
DMITIGR_WS_INLINE Listener_options& Listener_options::operator=(Listener_options&& rhs) = default;

DMITIGR_WS_INLINE std::unique_ptr<Listener_options> Listener_options::to_listener_options() const
{
  return std::make_unique<Listener_options>(*this);
}

DMITIGR_WS_INLINE const net::Endpoint_id* Listener_options::endpoint_id() const
{
  return rep_->endpoint_id();
}

DMITIGR_WS_INLINE Listener_options& Listener_options::set_idle_timeout(std::optional<std::chrono::milliseconds> value)
{
  rep_->set_idle_timeout(std::move(value));
  return *this;
}

DMITIGR_WS_INLINE std::optional<std::chrono::milliseconds> Listener_options::idle_timeout() const
{
  return rep_->idle_timeout();
}

DMITIGR_WS_INLINE Listener_options& Listener_options::set_max_payload_size(std::size_t value)
{
  rep_->set_max_payload_size(value);
  return *this;
}

DMITIGR_WS_INLINE std::size_t Listener_options::max_payload_size() const
{
  return rep_->max_payload_size();
}

DMITIGR_WS_API void Listener_options::swap(Listener_options& other)
{
  rep_.swap(other.rep_);
}

} // namespace dmitigr::ws

#include "dmitigr/ws/implementation_footer.hpp"
