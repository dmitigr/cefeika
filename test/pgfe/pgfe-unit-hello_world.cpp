// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#include <dmitigr/pgfe.hpp>
#include <cstdio>

namespace pgfe = dmitigr::pgfe;

int main() try {
  // Making the connection.
  pgfe::Connection conn{pgfe::Connection_options{pgfe::Communication_mode::net}
    .net_hostname("localhost").database("pgfe_test")
    .username("pgfe_test").password("pgfe_test")};

  // Connecting.
  conn.connect();

  // Using Pgfe's conversion function.
  using pgfe::to;

  // Executing query with positional parameters.
  conn.execute("select generate_series($1::int, $2::int)", 1, 3);
  while (conn.wait_response()) {
    if (const auto r = conn.row())
      std::printf("Number %i\n", to<int>(r.data()));
  }

  // Prepare and execute the statement with named parameters.
  auto* const ps = conn.prepare_statement("select :begin b, :end e");
  ps->set_parameter("begin", 0);
  ps->set_parameter("end", 1);
  ps->execute();
  auto r = ps->connection()->wait_row_then_discard();
  std::printf("Range [%i, %i]\n", to<int>(r.data("b")), to<int>(r.data("e")));

  // Invoking the function.
  conn.invoke("cos", .5f);
  r = conn.wait_row_then_discard();
  std::printf("cos(%f) = %f\n", .5f, to<float>(r.data()));

  // Provoking the syntax error.
  conn.perform("provoke syntax error");
  conn.wait_response_throw();
 } catch (const pgfe::c42_Syntax_error& e) {
  std::printf("Error %s is handled as expected.\n", e.error()->sqlstate());
 } catch (const std::exception& e) {
  std::printf("Oops: %s\n", e.what());
  return 1;
 }
