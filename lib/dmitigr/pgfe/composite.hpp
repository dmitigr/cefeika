// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#ifndef DMITIGR_PGFE_COMPOSITE_HPP
#define DMITIGR_PGFE_COMPOSITE_HPP

#include "dmitigr/pgfe/compositional.hpp"
#include "dmitigr/pgfe/conversions.hpp"
#include "dmitigr/pgfe/data.hpp"

#include <algorithm>
#include <cassert>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace dmitigr::pgfe {

/**
 * @ingroup main
 *
 * @brief A composite type.
 *
 * The implementation of Composite that stores the data as a vector.
 *
 * @remarks Fields removing will not invalidate pointers returned by data().
 */
struct Composite final : public Compositional {
  /// Default-constructible
  Composite() = default;

  /// See Composite::make().
  explicit Composite(std::vector<std::pair<std::string, std::unique_ptr<Data>>>&& datas) noexcept
    : datas{std::move(datas)}
  {
    assert(is_invariant_ok());
  }

  /// Copy-constructible.
  Composite(const Composite& rhs)
    : datas{rhs.datas.size()}
  {
    std::transform(cbegin(rhs.datas), cend(rhs.datas), begin(datas),
      [](const auto& pair) { return std::make_pair(pair.first, pair.second->to_data()); });

    assert(is_invariant_ok());
  }

  /// Copy-assignable.
  Composite& operator=(const Composite& rhs)
  {
    Composite tmp{rhs};
    swap(tmp);
    return *this;
  }

  /// Move-constructible.
  Composite(Composite&& rhs) = default;

  /// Move-assignable operator.
  Composite& operator=(Composite&& rhs) = default;

  /// Swaps the instances.
  void swap(Composite& rhs) noexcept
  {
    datas.swap(rhs.datas);
  }

  /// @see Composite::field_count.
  std::size_t field_count() const override
  {
    return datas.size();
  }

  /// @see Composite::has_fields.
  bool has_fields() const override
  {
    return !datas.empty();
  }

  /// @see Composite::field_name().
  const std::string& field_name(const std::size_t index) const override
  {
    assert(index < field_count());
    return datas[index].first;
  }

  /// @see Composite::field_index().
  std::optional<std::size_t> field_index(const std::string& name, const std::size_t offset = 0) const override
  {
    if (offset < field_count()) {
      const auto b = cbegin(datas);
      const auto e = cend(datas);
      const auto i = std::find_if(b + offset, e, [&name](const auto& pair) { return pair.first == name; });
      return (i != e) ? std::make_optional(i - b) : std::nullopt;
    } else
      return std::nullopt;
  }

  /// @see Composite::field_index_throw().
  std::size_t field_index_throw(const std::string& name, const std::size_t offset = 0) const override
  {
    const auto result = field_index(name, offset);
    assert(result);
    return *result;
  }

  /// @see Composite::has_field().
  bool has_field(const std::string& name, const std::size_t offset = 0) const override
  {
    return static_cast<bool>(field_index(name, offset));
  }

  /**
   * @returns The field data of this composite, or `nullptr` if NULL.
   *
   * @param index - see Compositional;
   *
   * @par Requires
   * `(index < field_count())`.
   */
  const std::unique_ptr<Data>& data(const std::size_t index) const noexcept
  {
    assert(index < field_count());
    return datas[index].second;
  }

  /**
   * @overload
   *
   * @param name - see Compositional;
   * @param offset - see Compositional.
   *
   * @par Requires
   * `has_field(name, offset)`.
   */
  const std::unique_ptr<Data>& data(const std::string& name, const std::size_t offset = 0) const noexcept
  {
    return data(field_index_throw(name, offset));
  }

  std::unique_ptr<Data>& data(const std::size_t index) noexcept
  {
    return const_cast<std::unique_ptr<Data>&>(static_cast<const Composite*>(this)->data(index));
  }

  std::unique_ptr<Data>& data(const std::string& name, const std::size_t offset = 0) noexcept
  {
    return const_cast<std::unique_ptr<Data>&>(static_cast<const Composite*>(this)->data(name, offset));
  }

  /**
   * @brief Sets the data of the specified index with the value of type T,
   * implicitly converted to the Data by using to_data().
   */
  template<typename T>
  std::enable_if_t<!std::is_same_v<Data*, T>> set_data(const std::size_t index, T&& value)
  {
    data(index) = to_data(std::forward<T>(value));
  }

  /// @overload
  template<typename T>
  std::enable_if_t<!std::is_same_v<Data*, T>> set_data(const std::string& name, T&& value)
  {
    set_data(field_index_throw(name), std::forward<T>(value));
  }

  /**
   * @brief Appends the field to this composite.
   *
   * @param name - see Compositional.
   * @param data - the data to set.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  void append(const std::string& name, std::unique_ptr<Data>&& data) noexcept
  {
    datas.emplace_back(name, std::move(data));
    assert(is_invariant_ok());
  }

  /// @overload
  template<typename T>
  void append(const std::string& name, T&& value)
  {
    append(name, to_data(std::forward<T>(value)));
  }

  /// Appends `rhs` to the end of the instance.
  void append(Composite&& rhs)
  {
    datas.insert(cend(datas), std::make_move_iterator(begin(rhs.datas)), std::make_move_iterator(end(rhs.datas)));
    assert(is_invariant_ok());
  }

  /**
   * @brief Inserts new field to this composite.
   *
   * @param index - the index of the field before which the new field will be inserted;
   * @param name - the name of the new field;
   * @param data - the data to set to the new field.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(index < field_count())`.
   */
  void insert(const std::size_t index, const std::string& name, std::unique_ptr<Data>&& data = {})
  {
    assert(index < field_count());
    datas.insert(begin(datas) + index, std::make_pair(name, std::move(data)));
    assert(is_invariant_ok());
  }

  /// @overload
  template<typename T>
  void insert(std::size_t index, const std::string& name, T&& value)
  {
    insert(index, name, to_data(std::forward<T>(value)));
  }

  /**
   * @overload
   *
   * @param name - the name of the field before which the new field will be inserted;
   * @param new_field_name - the name of the new field;
   * @param data - the data to set to the new field.
   *
   * @par Requires
   * `has_field(name, 0)`.
   */
  void insert(const std::string& name, const std::string& new_field_name, std::unique_ptr<Data>&& data)
  {
    insert(field_index_throw(name), new_field_name, std::move(data));
  }

  /// @overload
  template<typename T>
  void insert(const std::string& name, const std::string& new_field_name, T&& value)
  {
    insert(name, new_field_name, to_data(std::forward<T>(value)));
  }

  /**
   * @brief Removes field from this composite.
   *
   * @par Requires
   * `(index < field_count())`.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  void remove(const std::size_t index) noexcept
  {
    assert(index < field_count());
    datas.erase(cbegin(datas) + index);
    assert(is_invariant_ok());
  }

  /**
   * @overload
   *
   * @param name - see Compositional;
   * @param offset - see Compositional.
   *
   * @par Effects
   * `!has_field(name, offset)`.
   */
  void remove(const std::string& name, const std::size_t offset = 0) noexcept
  {
    if (const auto index = field_index(name, offset))
      datas.erase(cbegin(datas) + *index);
    assert(is_invariant_ok());
  }

  std::vector<std::pair<std::string, std::unique_ptr<Data>>> datas;
};

} // namespace dmitigr::pgfe

#endif  // DMITIGR_PGFE_COMPOSITE_HPP
