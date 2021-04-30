// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or ws.hpp

#include "dmitigr/ws/basics.hpp"
#include "dmitigr/ws/listener_options.hpp"
#include <dmitigr/net/listener.hpp>

#include <cassert>
#include <limits>
#include <stdexcept>

namespace dmitigr::ws::detail {

inline namespace validators {

inline void validate(const bool condition, const std::string& option_name)
{
  if (!condition)
    throw std::runtime_error{"invalid value of \"" + option_name + "\" connection option"};
}

} // inline namespace validators

/// The representation of Listener_options.
class iListener_options final {
public:
  iListener_options(std::string address, const int port, const int backlog)
    : net_options_{std::move(address), port, backlog}
  {
    assert(is_invariant_ok());
  }

  const net::Endpoint& endpoint() const
  {
    return net_options_.endpoint();
  }

  void set_idle_timeout(std::optional<std::chrono::milliseconds> value)
  {
    if (value)
      validate(value->count() >= 0, "connect timeout");
    idle_timeout_ = std::move(value);
    assert(is_invariant_ok());
  }

  std::optional<std::chrono::milliseconds> idle_timeout() const
  {
    return idle_timeout_;
  }

  void set_max_payload_size(const std::size_t value)
  {
    validate(value <= std::numeric_limits<int>::max(), "max payload size");
    max_payload_size_ = value;
    assert(is_invariant_ok());
  }

  std::size_t max_payload_size() const
  {
    return max_payload_size_;
  }

  void set_max_buffered_amount(const std::size_t value)
  {
    validate(value <= std::numeric_limits<int>::max(), "max buffered amount");
    max_buffered_amount_ = value;
    assert(is_invariant_ok());
  }

  std::size_t max_buffered_amount() const
  {
    return max_buffered_amount_;
  }

  void set_http_enabled(const bool value)
  {
    is_http_enabled_ = value;
  }

  bool is_http_enabled() const
  {
    return is_http_enabled_;
  }

  void set_ssl_enabled(const bool value)
  {
#ifndef DMITIGR_CEFEIKA_OPENSSL
    if (value)
      throw std::logic_error{"dmitigr::ws must be compiled with "
        "DMITIGR_CEFEIKA_OPENSSL in order to enable SSL"};
#endif
    is_ssl_enabled_ = value;
    assert(is_invariant_ok());
  }

  bool is_ssl_enabled() const
  {
    return is_ssl_enabled_;
  }

  void set_ssl_pem_file_password(std::optional<std::string> value)
  {
    assert(is_ssl_enabled());
    if (value)
      validate(!value->empty(), "SSL PEM file password");
    ssl_pem_file_password_ = std::move(value);
    assert(is_invariant_ok());
  }

  const std::optional<std::string>& ssl_pem_file_password() const
  {
    return ssl_pem_file_password_;
  }

  void set_ssl_certificate_file(std::optional<std::filesystem::path> value)
  {
    assert(is_ssl_enabled());
    if (value)
      validate(!value->empty(), "SSL certificate file");
    ssl_certificate_file_ = std::move(value);
    assert(is_invariant_ok());
  }

  const std::optional<std::filesystem::path>& ssl_certificate_file() const
  {
    return ssl_certificate_file_;
  }

  void set_ssl_private_key_file(std::optional<std::filesystem::path> value)
  {
    assert(is_ssl_enabled());
    if (value)
      validate(!value->empty(), "SSL private key file");
    ssl_private_key_file_ = std::move(value);
    assert(is_invariant_ok());
  }

  const std::optional<std::filesystem::path>& ssl_private_key_file() const
  {
    return ssl_private_key_file_;
  }

  void set_ssl_certificate_authority_file(std::optional<std::filesystem::path> value)
  {
    assert(is_ssl_enabled());
    if (value)
      validate(!value->empty(), "SSL certificate authority file");
    ssl_certificate_authority_file_ = std::move(value);
    assert(is_invariant_ok());
  }

  const std::optional<std::filesystem::path>& ssl_certificate_authority_file() const
  {
    return ssl_certificate_authority_file_;
  }

  void set_ssl_dh_parameters_file(std::optional<std::filesystem::path> value)
  {
    assert(is_ssl_enabled());
    if (value)
      validate(!value->empty(), "SSL Diffie-Hellman parameters file");
    ssl_dh_parameters_file_ = std::move(value);
    assert(is_invariant_ok());
  }

  const std::optional<std::filesystem::path>& ssl_dh_parameters_file() const
  {
    return ssl_dh_parameters_file_;
  }

private:
  friend iListener;
  friend Listener_options;

  net::Listener_options net_options_;
  std::optional<std::chrono::milliseconds> idle_timeout_;
  std::size_t max_payload_size_{static_cast<std::size_t>(std::numeric_limits<int>::max())};
  std::size_t max_buffered_amount_{static_cast<std::size_t>(std::numeric_limits<int>::max())};

  bool is_http_enabled_{};

  bool is_ssl_enabled_{};
  std::optional<std::string> ssl_pem_file_password_;
  std::optional<std::filesystem::path> ssl_certificate_file_;
  std::optional<std::filesystem::path> ssl_private_key_file_;
  std::optional<std::filesystem::path> ssl_certificate_authority_file_;
  std::optional<std::filesystem::path> ssl_dh_parameters_file_;

  bool is_invariant_ok() const
  {
    const bool net_ok = (net_options_.endpoint().communication_mode() == net::Communication_mode::net);
    const bool idle_timeout_ok = (!idle_timeout_ || idle_timeout_->count() >= 0);
    const bool max_payload_size_ok = (max_payload_size_ <= std::numeric_limits<int>::max());
    const bool ssl_ok =
      (!ssl_pem_file_password_ || !ssl_pem_file_password_->empty()) &&
      (!ssl_certificate_file_ || !ssl_certificate_file_->empty()) &&
      (!ssl_private_key_file_ || !ssl_private_key_file_->empty()) &&
      (!ssl_certificate_authority_file_ || !ssl_certificate_authority_file_->empty()) &&
      (!ssl_dh_parameters_file_ || !ssl_dh_parameters_file_->empty());

    return net_ok && idle_timeout_ok && max_payload_size_ok && ssl_ok;
  }
};

} // namespace dmitigr::ws::detail

namespace dmitigr::ws {

DMITIGR_WS_INLINE Listener_options::~Listener_options() = default;

DMITIGR_WS_INLINE Listener_options::Listener_options(std::string address, const int port)
  : rep_{std::make_unique<detail::iListener_options>(std::move(address), port, /*FIXME: backlog*/ 512)}
{
  assert(rep_ && rep_->is_invariant_ok());
}

DMITIGR_WS_INLINE Listener_options::Listener_options(const Listener_options& rhs)
  : rep_{std::make_unique<detail::iListener_options>(*rhs.rep_)}
{
  assert(rep_ && rep_->is_invariant_ok());
}

DMITIGR_WS_INLINE Listener_options& Listener_options::operator=(const Listener_options& rhs)
{
  Listener_options rhs_copy{rhs};
  swap(rhs_copy);
  assert(rep_ && rep_->is_invariant_ok());
  return *this;
}

DMITIGR_WS_INLINE Listener_options::Listener_options(Listener_options&& rhs) = default;
DMITIGR_WS_INLINE Listener_options& Listener_options::operator=(Listener_options&& rhs) = default;

DMITIGR_WS_INLINE std::unique_ptr<Listener_options> Listener_options::to_listener_options() const
{
  return std::make_unique<Listener_options>(*this);
}

DMITIGR_WS_INLINE const net::Endpoint& Listener_options::endpoint() const
{
  return rep_->endpoint();
}

DMITIGR_WS_INLINE Listener_options& Listener_options::set_http_enabled(const bool value)
{
  rep_->set_http_enabled(value);
  return *this;
}

DMITIGR_WS_INLINE bool Listener_options::is_http_enabled() const
{
  return rep_->is_http_enabled();
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

DMITIGR_WS_INLINE Listener_options& Listener_options::set_max_payload_size(const std::size_t value)
{
  rep_->set_max_payload_size(value);
  return *this;
}

DMITIGR_WS_INLINE std::size_t Listener_options::max_payload_size() const
{
  return rep_->max_payload_size();
}

DMITIGR_WS_INLINE Listener_options& Listener_options::set_max_buffered_amount(const std::size_t value)
{
  rep_->set_max_buffered_amount(value);
  return *this;
}

DMITIGR_WS_INLINE std::size_t Listener_options::max_buffered_amount() const
{
  return rep_->max_buffered_amount();
}

DMITIGR_WS_INLINE Listener_options& Listener_options::set_ssl_enabled(const bool value)
{
  rep_->set_ssl_enabled(value);
  return *this;
}

DMITIGR_WS_INLINE bool Listener_options::is_ssl_enabled() const
{
  return rep_->is_ssl_enabled();
}

DMITIGR_WS_INLINE Listener_options& Listener_options::set_ssl_pem_file_password(std::optional<std::string> value)
{
  rep_->set_ssl_pem_file_password(std::move(value));
  return *this;
}

DMITIGR_WS_INLINE const std::optional<std::string>& Listener_options::ssl_pem_file_password() const
{
  return rep_->ssl_pem_file_password();
}

DMITIGR_WS_INLINE Listener_options& Listener_options::set_ssl_certificate_file(std::optional<std::filesystem::path> value)
{
  rep_->set_ssl_certificate_file(std::move(value));
  return *this;
}

DMITIGR_WS_INLINE const std::optional<std::filesystem::path>& Listener_options::ssl_certificate_file() const
{
  return rep_->ssl_certificate_file();
}

DMITIGR_WS_INLINE Listener_options& Listener_options::set_ssl_private_key_file(std::optional<std::filesystem::path> value)
{
  rep_->set_ssl_private_key_file(std::move(value));
  return *this;
}

DMITIGR_WS_INLINE const std::optional<std::filesystem::path>& Listener_options::ssl_private_key_file() const
{
  return rep_->ssl_private_key_file();
}

DMITIGR_WS_INLINE Listener_options& Listener_options::set_ssl_certificate_authority_file(std::optional<std::filesystem::path> value)
{
  rep_->set_ssl_certificate_authority_file(std::move(value));
  return *this;
}

DMITIGR_WS_INLINE const std::optional<std::filesystem::path>& Listener_options::ssl_certificate_authority_file() const
{
  return rep_->ssl_certificate_authority_file();
}

DMITIGR_WS_INLINE Listener_options& Listener_options::set_ssl_dh_parameters_file(std::optional<std::filesystem::path> value)
{
  rep_->set_ssl_dh_parameters_file(std::move(value));
  return *this;
}

DMITIGR_WS_INLINE const std::optional<std::filesystem::path>& Listener_options::ssl_dh_parameters_file() const
{
  return rep_->ssl_dh_parameters_file();
}

DMITIGR_WS_INLINE void Listener_options::swap(Listener_options& other)
{
  rep_.swap(other.rep_);
}

} // namespace dmitigr::ws
