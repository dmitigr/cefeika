// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#ifndef DMITIGR_PGFE_ENTITY_VECTOR_HPP
#define DMITIGR_PGFE_ENTITY_VECTOR_HPP

#include "dmitigr/pgfe/conversions_api.hpp"
#include "dmitigr/pgfe/dll.hpp"
#include "dmitigr/pgfe/sql_string.hpp"
#include "dmitigr/pgfe/types_fwd.hpp"
#include "dmitigr/pgfe/implementation_header.hpp"

#include "dmitigr/util/debug.hpp"

#include <cstddef>
#include <memory>
#include <vector>

namespace dmitigr::pgfe {

/**
 * @ingroup conversions
 *
 * @brief An entity container.
 *
 * This template is intented for automatic conversion of set of objects to
 * a vector of application level abstractions. This is done by applying the
 * conversion routine `Conversions<Entity>::to_type()` to each object.
 */
template<class Entity>
class Entity_vector final {
public:
  using Value_type = Entity;

  /// @name Constructors
  /// @{

  /**
   * @brief Constructs an empty entity vector.
   */
  Entity_vector() = default;

  /**
   * @brief Constructs an entity vector from `std::vector<Entity>`.
   */
  Entity_vector(std::vector<Entity>&& entities)
    : entities_{std::move(entities)}
  {}

  /**
   * @brief Constructs an entity vector from the vector of objects.
   *
   * @par Requires
   * The conversion routine `Conversions<Entity>::to_type(Object&&)`
   * must be defined.
   *
   * @see Row.
   */
  template<typename Object>
  Entity_vector(std::vector<Object>&& objects)
  {
    entities_.reserve(objects.size());
    for (auto& obj : objects)
      entities_.emplace_back(to<Entity>(std::move(obj)));
  }

  /**
   * @brief Constructs a vector of entities from the rows returned by the
   * server during the `statement` execution.
   *
   * @par Requires
   * The conversion routine `Conversions<Entity>::to_type(const Row*)`
   * must be defined.
   */
  template<typename ... Types>
  Entity_vector(Connection* const conn, const Sql_string* const statement, Types&& ... parameters)
  {
    DMITIGR_REQUIRE(conn && statement, std::invalid_argument,
      "nullptr has been passed to the constructor of dmitigr::pgfe::Entity_vector");
    conn->execute(statement, std::forward<Types>(parameters)...);
    entities_.reserve(16);
    conn->for_each([&](const Row* const row) {
      if (!(entities_.size() < entities_.capacity()))
        entities_.reserve(entities_.capacity() * 2);
      entities_.emplace_back(to<Entity>(row));
    });
    entities_.shrink_to_fit();
  }

  /**
   * @overload
   */
  template<typename ... Types>
  Entity_vector(Connection* const conn, const std::string& statement, Types&& ... parameters)
    : Entity_vector{conn, Sql_string::make(statement).get(), std::forward<Types>(parameters)...}
  {}

  /// @}

  // ===========================================================================

  /**
   * @returns The number of entities.
   */
  std::size_t entity_count() const
  {
    return entities_.size();
  }

  /**
   * @returns `true` if this vector is empty, or `false` otherwise.
   */
  bool has_entities() const
  {
    return entities_.empty();
  }

  /**
   * @returns The entity of this vector.
   *
   * @par Requires
   * `(index < entity_count())`.
   *
   * @see entity_count(), release_entity().
   */
  const Entity& entity(const std::size_t index) const
  {
    check_index(index);
    return entities_[index];
  }

  /**
   * @returns The entity of this vector.
   *
   * @remarks The behaviour is undefined if `(index >= entity_count())`.
   *
   * @see entity(), entity_count().
   */
  const Entity& operator[](const std::size_t index) const
  {
    return entities_[index];
  }

  /**
   * @brief Sets the specified entity at the specified index of the vector.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(index < entity_count())`.
   */
  template<class E>
  void set_entity(const std::size_t index, E&& entity)
  {
    check_index(index);
    entities_[index] = std::forward<E>(entity);
  }

  /**
   * @brief Release the entity from this vector.
   *
   * @returns The released object of type Entity.
   *
   * @par Effects
   * Entity at `index` is the default constructed entity.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(index < entity_count())`.
   */
  Entity release_entity(const std::size_t index)
  {
    check_index(index);
    auto& entity = entities_[index];
    auto result = std::move(entity); // As described in 14882:2014 20.8.1/4, u.p is equal to nullptr after transfer ownership.
    entity = Entity{};
    return result;
  }

  /**
   * @returns The value of type `std::vector<Entity>`.
   *
   * @par Effects
   * `!has_entities()`.
   */
  std::vector<Entity> release()
  {
    std::vector<Entity> result;
    entities_.swap(result);
    return std::move(result);
  }

private:
  void check_index(const std::size_t index) const
  {
    DMITIGR_REQUIRE(index < entity_count(), std::out_of_range,
      "invalid entity index (" + std::to_string(index) + ") of the pgfe::Entity_vector instance");
  }

  std::vector<Entity> entities_;
};

} // namespace dmitigr::pgfe

#endif  // DMITIGR_PGFE_ENTITY_VECTOR_HPP

#include "dmitigr/pgfe/implementation_footer.hpp"
