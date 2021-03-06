// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#ifndef DMITIGR_PGFE_COMPOSITE_HPP
#define DMITIGR_PGFE_COMPOSITE_HPP

#include "compositional.hpp"
#include "conversions.hpp"
#include "data.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
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
    std::transform(rhs.datas_.cbegin(), rhs.datas_.cend(), datas_.begin(),
      [](const auto& pair) { return std::make_pair(pair.first, pair.second->to_data()); });

    assert(is_invariant_ok());
  }

  /// Copy-assignable.
  Composite& operator=(const Composite& rhs)
  {
    if (this != &rhs) {
      Composite tmp{rhs};
      swap(tmp);
    }
    return *this;
  }

  /// Move-constructible.
  Composite(Composite&& rhs) = default;

  /// Move-assignable.
  Composite& operator=(Composite&& rhs) = default;

  /// Swaps the instances.
  void swap(Composite& rhs) noexcept
  {
    datas_.swap(rhs.datas_);
  }

  /// @see Compositional::size().
  std::size_t size() const noexcept override
  {
    return datas_.size();
  }

  /// @see Compositional::is_empty().
  bool is_empty() const noexcept override
  {
    return datas_.empty();
  }

  /// @see Compositional::name_of().
  std::string_view name_of(const std::size_t index) const noexcept override
  {
    assert(index < size());
    return datas_[index].first;
  }

  /// @see Compositional::index_of().
  std::size_t index_of(const std::string_view name, const std::size_t offset = 0) const noexcept override
  {
    const auto sz = size();
    const auto b = datas_.cbegin();
    const auto e = datas_.cend();
    using Diff = decltype(b)::difference_type;
    const auto i = std::find_if(std::min(b + static_cast<Diff>(offset), b + static_cast<Diff>(sz)), e,
      [&name](const auto& pair) { return pair.first == name; });
    return static_cast<std::size_t>(i - b);
  }

  /**
   * @returns The field data of this composite, or `nullptr` if NULL.
   *
   * @param index See Compositional.
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
   * @param name See Compositional.
   * @param offset See Compositional.
   *
   * @par Requires
   * `has_field(name, offset)`.
   */
  const std::unique_ptr<Data>& data(const std::string_view name, const std::size_t offset = 0) const
  {
    return data(index_of(name, offset));
  }

  /// @overload
  std::unique_ptr<Data>& data(const std::string_view name, const std::size_t offset = 0) noexcept
  {
    return const_cast<std::unique_ptr<Data>&>(static_cast<const Composite*>(this)->data(name, offset));
  }

  /// @returns `data(index)`.
  const auto& operator[](const std::size_t index) const noexcept
  {
    return data(index);
  }

  /// @overload
  auto& operator[](const std::size_t index) noexcept
  {
    return const_cast<std::unique_ptr<Data>&>(static_cast<const Composite*>(this)->operator[](index));
  }

  /// @returns `data(name)`.
  const auto& operator[](const std::string_view name) const noexcept
  {
    return data(name);
  }

  /// @overload
  auto& operator[](const std::string_view name) noexcept
  {
    return const_cast<std::unique_ptr<Data>&>(static_cast<const Composite*>(this)->operator[](name));
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
  std::enable_if_t<!std::is_same_v<Data*, T>> set_data(const std::string_view name, T&& value)
  {
    set_data(index_of(name), std::forward<T>(value));
  }

  /**
   * @brief Appends the field to this composite.
   *
   * @param name See Compositional.
   * @param data A data to set.
   *
   * @par Exception safety guarantee
   * Strong.
   */
  void append(std::string name, std::unique_ptr<Data>&& data) noexcept
  {
    datas_.emplace_back(std::move(name), std::move(data));
    assert(is_invariant_ok());
  }

  /// @overload
  template<typename T>
  void append(std::string name, T&& value)
  {
    append(std::move(name), to_data(std::forward<T>(value)));
  }

  /// Appends `rhs` to the end of the instance.
  void append(Composite&& rhs)
  {
    datas_.insert(datas_.cend(),
      std::make_move_iterator(rhs.datas_.begin()),
      std::make_move_iterator(rhs.datas_.end()));
    assert(is_invariant_ok());
  }

  /**
   * @brief Inserts new field to this composite.
   *
   * @param index An index of a field before which
   * a new field will be inserted.
   * @param name A name of a new field.
   * @param data A data of a new field.
   *
   * @par Exception safety guarantee
   * Strong.
   *
   * @par Requires
   * `(index < size())`.
   */
  void insert(const std::size_t index, std::string name, std::unique_ptr<Data>&& data = {})
  {
    assert(index < size());
    const auto b = datas_.begin();
    using Diff = decltype(b)::difference_type;
    datas_.insert(b + static_cast<Diff>(index),
      std::make_pair(std::move(name), std::move(data)));
    assert(is_invariant_ok());
  }

  /// @overload
  template<typename T>
  void insert(const std::size_t index, std::string name, T&& value)
  {
    insert(index, std::move(name), to_data(std::forward<T>(value)));
  }

  /**
   * @overload
   *
   * @param name A name of a field before which a new field will be inserted.
   * @param new_field_name A name of a new field.
   * @param data A data of a new field.
   *
   * @par Requires
   * `has_field(name, 0)`.
   */
  void insert(const std::string_view name, std::string new_field_name, std::unique_ptr<Data>&& data)
  {
    insert(index_of(name), std::move(new_field_name), std::move(data));
  }

  /// @overload
  template<typename T>
  void insert(const std::string_view name, std::string new_field_name, T&& value)
  {
    insert(name, std::move(new_field_name), to_data(std::forward<T>(value)));
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
    const auto b = datas_.cbegin();
    using Diff = decltype(b)::difference_type;
    datas_.erase(b + static_cast<Diff>(index));
    assert(is_invariant_ok());
  }

  /**
   * @overload
   *
   * @param name See Compositional.
   * @param offset See Compositional.
   *
   * @par Effects
   * `!has_field(name, offset)`.
   */
  void remove(const std::string_view name, const std::size_t offset = 0) noexcept
  {
    if (const auto index = index_of(name, offset); index != size()) {
      const auto b = datas_.cbegin();
      using Diff = decltype(b)::difference_type;
      datas_.erase(b + static_cast<Diff>(index));
    }
    assert(is_invariant_ok());
  }

  /// @returns The iterator that points to the first field.
  auto begin() noexcept
  {
    return datas_.begin();
  }

  /// @returns The constant iterator that points to the first.
  auto begin() const noexcept
  {
    return datas_.begin();
  }

  /// @returns The constant iterator that points to the first field.
  auto cbegin() const noexcept
  {
    return datas_.cbegin();
  }

  /// @returns The iterator that points to the one-past-the-last field.
  auto end() noexcept
  {
    return datas_.end();
  }

  /// @returns The constant iterator that points to the one-past-the-last field.
  auto end() const noexcept
  {
    return datas_.end();
  }

  /// @returns The constant iterator that points to the one-past-the-last field.
  auto cend() const noexcept
  {
    return datas_.cend();
  }

private:
  std::vector<std::pair<std::string, std::unique_ptr<Data>>> datas_;
};

/// Composite is swappable.
inline void swap(Composite& lhs, Composite& rhs) noexcept
{
  lhs.swap(rhs);
}

/**
 * @returns
 *   - negative value if the first differing field in `lhs` is less than the
 *   corresponding field in `rhs`;
 *   - zero if all fields of `lhs` and `rhs` are equal;
 *   - positive value if the first differing field in `lhs` is greater than the
 *   corresponding field in `rhs`.
 */
inline int cmp(const Composite& lhs, const Composite& rhs) noexcept
{
  if (const auto lsz = lhs.size(), rsz = rhs.size(); lsz == rsz) {
    for (auto i = 0*lsz; i < lsz; ++i) {
      if (lhs.name_of(i) < rhs.name_of(i) || *lhs[i] < *rhs[i])
        return -1;
      else if (lhs.name_of(i) > rhs.name_of(i) || *lhs[i] > *rhs[i])
        return 1;
    }
    return 0;
  } else
    return lsz < rsz ? -1 : 1;
}

/**
 * @returns `cmp(lhs, rhs) < 0`.
 *
 * @see cmp(const Composite&, const Composite&).
 */
inline bool operator<(const Composite& lhs, const Composite& rhs) noexcept
{
  return cmp(lhs, rhs) < 0;
}

/**
 * @returns `cmp(lhs, rhs) <= 0`.
 *
 * @see cmp(const Composite&, const Composite&).
 */
inline bool operator<=(const Composite& lhs, const Composite& rhs) noexcept
{
  return cmp(lhs, rhs) <= 0;
}

/**
 * @returns `cmp(lhs, rhs) == 0`.
 *
 * @see cmp(const Composite&, const Composite&).
 */
inline bool operator==(const Composite& lhs, const Composite& rhs) noexcept
{
  return !cmp(lhs, rhs);
}

/**
 * @returns `cmp(lhs, rhs) != 0`.
 *
 * @see cmp(const Composite&, const Composite&).
 */
inline bool operator!=(const Composite& lhs, const Composite& rhs) noexcept
{
  return !(lhs == rhs);
}

/**
 * @returns `cmp(lhs, rhs) > 0`.
 *
 * @see cmp(const Composite&, const Composite&).
 */
inline bool operator>(const Composite& lhs, const Composite& rhs) noexcept
{
  return cmp(lhs, rhs) > 0;
}

/**
 * @returns `cmp(lhs, rhs) >= 0`.
 *
 * @see cmp(const Composite&, const Composite&).
 */
inline bool operator>=(const Composite& lhs, const Composite& rhs) noexcept
{
  return cmp(lhs, rhs) >= 0;
}

} // namespace dmitigr::pgfe

#endif  // DMITIGR_PGFE_COMPOSITE_HPP
