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
class Composite final : public Compositional {
public:
  /// Default-constructible
  Composite() = default;

  /// See Composite::make().
  explicit Composite(std::vector<std::pair<std::string, std::unique_ptr<Data>>>&& datas) noexcept
    : datas_{std::move(datas)}
  {
    assert(is_invariant_ok());
  }

  /// Copy-constructible.
  Composite(const Composite& rhs)
    : datas_{rhs.datas_.size()}
  {
    std::transform(cbegin(rhs.datas_), cend(rhs.datas_), begin(datas_),
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
    datas_.swap(rhs.datas_);
  }

  std::size_t size() const noexcept override
  {
    return datas_.size();
  }

  bool empty() const noexcept override
  {
    return datas_.empty();
  }

  const std::string& name_of(const std::size_t index) const noexcept override
  {
    assert(index < size());
    return datas_[index].first;
  }

  std::size_t index_of(const std::string& name, const std::size_t offset = 0) const noexcept override
  {
    if (offset < size()) {
      const auto b = cbegin(datas_);
      const auto e = cend(datas_);
      const auto i = std::find_if(b + offset, e, [&name](const auto& pair) { return pair.first == name; });
      return (i != e) ? (i - b) : nidx;
    } else
      return nidx;
  }

  /**
   * @returns The field data of this composite, or `nullptr` if NULL.
   *
   * @param index - see Compositional;
   *
   * @par Requires
   * `(index < size())`.
   */
  const std::unique_ptr<Data>& data(const std::size_t index) const noexcept
  {
    assert(index < size());
    return datas_[index].second;
  }

  /// @overload
  std::unique_ptr<Data>& data(const std::size_t index) noexcept
  {
    return const_cast<std::unique_ptr<Data>&>(static_cast<const Composite*>(this)->data(index));
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
  const std::unique_ptr<Data>& data(const std::string& name, const std::size_t offset = 0) const
  {
    return data(index_of(name, offset));
  }

  /// @overload
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
    set_data(index_of(name), std::forward<T>(value));
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
    datas_.emplace_back(name, std::move(data));
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
    datas_.insert(cend(datas_),
      std::make_move_iterator(begin(rhs.datas_)),
      std::make_move_iterator(end(rhs.datas_)));
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
   * `(index < size())`.
   */
  void insert(const std::size_t index, const std::string& name, std::unique_ptr<Data>&& data = {})
  {
    assert(index < size());
    datas_.insert(begin(datas_) + index, std::make_pair(name, std::move(data)));
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
    insert(index_of(name), new_field_name, std::move(data));
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
   * `(index < size())`.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  void remove(const std::size_t index) noexcept
  {
    assert(index < size());
    datas_.erase(cbegin(datas_) + index);
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
    if (const auto index = index_of(name, offset); index != nidx)
      datas_.erase(cbegin(datas_) + index);
    assert(is_invariant_ok());
  }

private:
  std::vector<std::pair<std::string, std::unique_ptr<Data>>> datas_;
};

} // namespace dmitigr::pgfe

#endif  // DMITIGR_PGFE_COMPOSITE_HPP
