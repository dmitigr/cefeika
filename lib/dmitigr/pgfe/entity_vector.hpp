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
#include <iterator>
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
  /** The alias of entity type. */
  using Value_type = Entity;

  /** The alias of underlying container type. */
  using Underlying_container_type = std::vector<Entity>;

  /** The alias of iterator. */
  using Iterator = typename Underlying_container_type::iterator;

  /** The alias of iterator of constant. */
  using Const_iterator = typename Underlying_container_type::const_iterator;

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
  Entity& entity(const std::size_t index)
  {
    check_index(index);
    return entities_[index];
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
  Entity& operator[](const std::size_t index)
  {
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
   * @brief Appends the specified entity to the end of the vector.
   *
   * @par Requires
   * The conversion routine `Conversions<Entity>::to_type(Object&&)`
   * must be defined.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  template<class Object>
  void append_entity(Object&& object)
  {
    entities_.emplace_back(to<Entity>(std::forward<Object>(object)));
  }

  /**
   * @brief Appends the specified entity to the end of the vector.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  void append_entity(const Entity& entity)
  {
    entities_.push_back(entity);
  }

  /**
   * @overload
   */
  void append_entity(Entity&& entity)
  {
    entities_.emplace_back(std::move(entity));
  }

  /**
   * @brief Removes entity from this vector.
   *
   * @par Requires
   * `(index < entity_count())`.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  void remove_entity(const std::size_t index)
  {
    check_index(index);
    entities_.erase(cbegin(entities_) + index);
  }

  /**
   * @overload
   */
  void remove_entity(const Const_iterator i)
  {
    check_iterator(i);
    entities_.erase(i);
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

  // ===========================================================================

  /**
   * @returns The iterator that points to the first entity.
   */
  Iterator begin()
  {
    return entities_.begin();
  }

  /**
   * @returns The iterator that points to the element after the last entity.
   */
  Iterator end()
  {
    return entities_.end();
  }

  /**
   * @returns The constant iterator that points to the first entity.
   */
  Const_iterator cbegin() const
  {
    return entities_.cbegin();
  }

  /**
   * @returns The constant iterator that points to the element after the last
   * entity.
   */
  Const_iterator cend() const
  {
    return entities_.cend();
  }

private:
  void check_index(const std::size_t index) const
  {
    DMITIGR_REQUIRE(index < entity_count(), std::out_of_range,
      "invalid entity index (" + std::to_string(index) + ") of the pgfe::Entity_vector instance");
  }

  void check_iterator(const Const_iterator i) const
  {
    DMITIGR_REQUIRE(i < cend(), std::out_of_range,
      "invalid entity iterator of the pgfe::Entity_vector instance");
  }

  std::vector<Entity> entities_;
};

// =============================================================================

/**
 * @returns `v.begin()`.
 */
template<typename Entity>
auto begin(Entity_vector<Entity>& v)
{
  return v.begin();
}

/**
 * @returns `v.end()`.
 */
template<typename Entity>
auto end(Entity_vector<Entity>& v)
{
  return v.end();
}

/**
 * @returns `v.cbegin()`.
 */
template<typename Entity>
auto cbegin(const Entity_vector<Entity>& v)
{
  return v.cbegin();
}

/**
 * @returns `v.cend()`.
 */
template<typename Entity>
auto cend(const Entity_vector<Entity>& v)
{
  return v.cend();
}

} // namespace dmitigr::pgfe

#endif  // DMITIGR_PGFE_ENTITY_VECTOR_HPP

#include "dmitigr/pgfe/implementation_footer.hpp"
