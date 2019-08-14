// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#include "pgfe-unit.hpp"

#include "dmitigr/pgfe.hpp"

struct Person {
  int id;
  std::string name;
  unsigned int age;
};

namespace dmitigr::pgfe {

template<> struct Conversions<Person> {
  static Person to_type(const Row* const r)
  {
    ASSERT(r);
    Person p;
    p.id   = to<int>(r->data("id"));
    p.name = to<std::string>(r->data("name"));
    p.age  = to<unsigned int>(r->data("age"));
    return p;
  }

  static Person to_type(std::unique_ptr<Composite>&& c)
  {
    ASSERT(c);
    Person p;
    p.id   = to<int>(c->data("id"));
    p.name = to<std::string>(c->data("name"));
    p.age  = to<unsigned int>(c->data("age"));
    return p;
  }
};

} // namespace dmitigr::pgfe

int main(int, char* argv[])
{
  namespace pgfe = dmitigr::pgfe;
  using namespace dmitigr::test;

  try {
    // Connecting.
    const auto conn = pgfe::test::make_connection();
    conn->connect();

    // Preparing to test -- creating and filling the test table.
    conn->execute(R"(create temp table person(id serial not null primary key,
                                              name text not null,
                                              age integer not null))");
    conn->execute(R"(insert into person (name, age) values('Alla', 30),('Bella', 33))");

    // Test 1.
    {
      std::cout << "From rows created on the server side:";
      pgfe::Entity_vector<Person> persons{conn.get(), "select * from person"};
      for (std::size_t i = 0; i < persons.entity_count(); ++i) {
        std::cout << "Person " << i << " {\n";
        std::cout << "id: " << persons[i].id << "\n";
        std::cout << "name: " << persons[i].name << "\n";
        std::cout << "age: " << persons[i].age << "\n";
        std::cout << "}\n";
      }
    }

    // Test 2.
    {
      std::cout << "From composite created on the client side:";
      auto alla = pgfe::Composite::make();
      alla->append_field("id",  1);
      alla->append_field("name", "Alla");
      alla->append_field("age", "30");
      auto bella = pgfe::Composite::make();
      bella->append_field("id",  2);
      bella->append_field("name", "Bella");
      bella->append_field("age", "33");
      std::vector<decltype(alla)> pv;
      pv.emplace_back(std::move(alla));
      pv.emplace_back(std::move(bella));
      pgfe::Entity_vector<Person> persons{std::move(pv)};
      for (std::size_t i = 0; i < persons.entity_count(); ++i) {
        std::cout << "Person " << i << " {\n";
        std::cout << "id: " << persons[i].id << "\n";
        std::cout << "name: " << persons[i].name << "\n";
        std::cout << "age: " << persons[i].age << "\n";
        std::cout << "}\n";
      }

      const Person daria{3, "Daria", 35};
      persons.set_entity(0, daria);
    }
  } catch (const std::exception& e) {
    report_failure(argv[0], e);
    return 1;
  } catch (...) {
    report_failure(argv[0]);
    return 1;
  }
}
