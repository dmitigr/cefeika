// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or http.hpp

#include "dmitigr/http/exceptions.hpp"
#include "dmitigr/http/implementation_header.hpp"

namespace dmitigr::http {

Server_exception::Server_exception(const std::error_code ec)
  : system_error{ec}
{}

Server_exception::Server_exception(const std::error_code ec, const std::string& what)
  : system_error{ec, what}
{}

} // namespace dmitigr::http

#include "dmitigr/http/implementation_footer.hpp"
