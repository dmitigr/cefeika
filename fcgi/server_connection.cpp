// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or fcgi.hpp

#include "../assert.hpp"
#include "basics.hpp"
#include "server_connection.hpp"

namespace dmitigr::fcgi::detail {

/**
 * @brief The base implementation of the Server_connection.
 */
class iServer_connection : public Server_connection {
public:
  /**
   * @brief The constructor.
   */
  explicit iServer_connection(std::unique_ptr<net::Descriptor> io, const Role role,
    const int request_id, const bool is_keep_connection)
    : is_keep_connection_{is_keep_connection}
    , role_{role}
    , request_id_{request_id}
  {
    io_ = std::move(io);
    DMITIGR_ASSERT(io_);
  }

  // ---------------------------------------------------------------------------
  // Connection overridings
  // ---------------------------------------------------------------------------

  int request_id() const override
  {
    return request_id_;
  }

  Role role() const override
  {
    return role_;
  }

  std::size_t parameter_count() const override
  {
    return parameters_.pair_count();
  }

  std::optional<std::size_t> parameter_index(const std::string_view name) const override
  {
    return parameters_.pair_index(name);
  }

  std::size_t parameter_index_throw(const std::string_view name) const override
  {
    const auto result = parameter_index(name);
    DMITIGR_ASSERT(result);
    return *result;
  }

  const detail::Name_value* parameter(const std::size_t index) const override
  {
    DMITIGR_CHECK_RANGE(index < parameter_count());
    return parameters_.pair(index);
  }

  const detail::Name_value* parameter(const std::string_view name) const override
  {
    const auto index = parameter_index_throw(name);
    return parameters_.pair(index);
  }

  bool has_parameter(const std::string_view name) const override
  {
    return static_cast<bool>(parameter_index(name));
  }

  bool has_parameters() const override
  {
    return parameters_.has_pairs();
  }

  // ---------------------------------------------------------------------------
  // Server_connection overridings
  // ---------------------------------------------------------------------------

  int application_status() const override
  {
    return application_status_;
  }

  void set_application_status(const int status) override
  {
    application_status_ = status;
  }

  bool is_keep_connection() const
  {
    return is_keep_connection_;
  }

private:
  friend server_Istream;
  friend server_Streambuf;

  bool is_keep_connection_{};
  Role role_{};
  int request_id_{};
  int application_status_{};
  std::unique_ptr<net::Descriptor> io_;
  detail::Names_values parameters_;
};

} // namespace dmitigr::fcgi::detail
