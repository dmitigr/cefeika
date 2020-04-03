// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#include "dmitigr/pgfe/composite.hpp"
#include "dmitigr/pgfe/compositional.hpp"
#include "dmitigr/pgfe/data.hpp"
#include "dmitigr/pgfe/util.hpp"
#include <dmitigr/util/debug.hpp>

#include <algorithm>
#include <utility>
#include <vector>

namespace dmitigr::pgfe::detail {

/**
 * @brief The base implementation of Composite.
 */
class iComposite : public Composite {
protected:
  virtual bool is_invariant_ok() = 0;
};

inline bool iComposite::is_invariant_ok()
{
  const bool compositional_ok = detail::is_invariant_ok(*this);
  return compositional_ok;
}

/**
 * @brief The implementation of Composite that stores the data as a vector of
 * unique pointers.
 *
 * @remarks Fields removing will not invalidate pointers returned by data().
 */
class heap_data_Composite final : public iComposite {
public:
  /**
   * @brief The default constructor
   */
  heap_data_Composite() = default;

  /**
   * @brief See Composite::make().
   */
  explicit heap_data_Composite(std::vector<std::pair<std::string, std::unique_ptr<Data>>>&& datas)
    : datas_{std::move(datas)}
  {
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @brief The copy constructor.
   */
  heap_data_Composite(const heap_data_Composite& rhs)
    : datas_{rhs.datas_.size()}
  {
    std::transform(cbegin(rhs.datas_), cend(rhs.datas_), begin(datas_),
      [&](const auto& pair) { return std::make_pair(pair.first, pair.second->to_data()); });
    DMITIGR_ASSERT(is_invariant_ok());
  }

  /**
   * @brief The move constructor.
   */
  heap_data_Composite(heap_data_Composite&& rhs) = default;

  /**
   * @brief The copy assignment operator.
   */
  heap_data_Composite& operator=(const heap_data_Composite& rhs)
  {
    heap_data_Composite tmp{rhs};
    swap(tmp);
    return *this;
  }

  /**
   * @brief The move assignment operator.
   */
  heap_data_Composite& operator=(heap_data_Composite&& rhs) = default;

  /**
   * @brief The swap operation.
   */
  void swap(heap_data_Composite& rhs) noexcept
  {
    datas_.swap(rhs.datas_);
  }

  // ---------------------------------------------------------------------------
  // Compositional overridings
  // ---------------------------------------------------------------------------

  std::size_t field_count() const override
  {
    return datas_.size();
  }

  bool has_fields() const override
  {
    return !datas_.empty();
  }

  const std::string& field_name(const std::size_t index) const override
  {
    DMITIGR_REQUIRE(index < field_count(), std::out_of_range);
    return datas_[index].first;
  }

  std::optional<std::size_t> field_index(const std::string& name, const std::size_t offset = 0) const override
  {
    if (offset < field_count()) {
      const auto b = cbegin(datas_);
      const auto e = cend(datas_);
      const auto ident = unquote_identifier(name);
      const auto i = std::find_if(b + offset, e, [&](const auto& pair) { return pair.first == ident; });
      return i != e ? std::make_optional(i - b) : std::nullopt;
    } else
      return std::nullopt;
  }

  std::size_t field_index_throw(const std::string& name, const std::size_t offset = 0) const override
  {
    const auto result = field_index(name, offset);
    DMITIGR_REQUIRE(result, std::out_of_range,
      "the instance of dmitigr::pgfe::Composite has no field \"" + name + "\"");
    return *result;
  }

  bool has_field(const std::string& name, const std::size_t offset = 0) const override
  {
    return static_cast<bool>(field_index(name, offset));
  }

  // ---------------------------------------------------------------------------
  // Composite overridings
  // ---------------------------------------------------------------------------

  std::unique_ptr<Composite> to_composite() const override
  {
    return std::make_unique<heap_data_Composite>(*this);
  }

  const Data* data(const std::size_t index) const override
  {
    DMITIGR_REQUIRE(index < field_count(), std::out_of_range);
    return datas_[index].second.get();
  }

  const Data* data(const std::string& name, const std::size_t offset) const override
  {
    return data(field_index_throw(name, offset));
  }

  void set_data(const std::size_t index, std::unique_ptr<Data>&& data) override
  {
    DMITIGR_REQUIRE(index < field_count(), std::out_of_range);
    datas_[index].second = std::move(data);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  void set_data(const std::size_t index, std::nullptr_t) override
  {
    set_data(index, std::unique_ptr<Data>{});
  }

  void set_data(const std::string& name, std::unique_ptr<Data>&& data) override
  {
    set_data(field_index_throw(name), std::move(data));
  }

  void set_data(const std::string& name, std::nullptr_t) override
  {
    set_data(name, std::unique_ptr<Data>{});
  }

  std::unique_ptr<Data> release_data(const std::size_t index) override
  {
    DMITIGR_REQUIRE(index < field_count(), std::out_of_range);
    auto& data = datas_[index].second;
    auto result = std::move(data); // As described in 14882:2014 20.8.1/4, u.p is equal to nullptr after transfer ownership...
    data.reset(); // but just in case...
    DMITIGR_ASSERT(is_invariant_ok());
    return result;
  }

  std::unique_ptr<Data> release_data(const std::string& name, const std::size_t offset = 0) override
  {
    return release_data(field_index_throw(name, offset));
  }

  void append_field(const std::string& name, std::unique_ptr<Data>&& data = {}) override
  {
    datas_.emplace_back(name, std::move(data));
    DMITIGR_ASSERT(is_invariant_ok());
  }

  void insert_field(const std::size_t index, const std::string& name, std::unique_ptr<Data>&& data = {}) override
  {
    DMITIGR_REQUIRE(index < field_count(), std::out_of_range);
    datas_.insert(begin(datas_) + index, std::make_pair(name, std::move(data)));
    DMITIGR_ASSERT(is_invariant_ok());
  }

  void insert_field(const std::string& name, const std::string& new_field_name, std::unique_ptr<Data>&& data) override
  {
    insert_field(field_index_throw(name), new_field_name, std::move(data));
  }

  void remove_field(const std::size_t index) override
  {
    DMITIGR_REQUIRE(index < field_count(), std::out_of_range);
    datas_.erase(cbegin(datas_) + index);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  void remove_field(const std::string& name, const std::size_t offset) override
  {
    if (const auto index = field_index(name, offset))
      datas_.erase(cbegin(datas_) + *index);
    DMITIGR_ASSERT(is_invariant_ok());
  }

  std::vector<std::pair<std::string, std::unique_ptr<Data>>> to_vector() const override
  {
    heap_data_Composite copy{*this};
    return std::move(copy.datas_);
  }

  std::vector<std::pair<std::string, std::unique_ptr<Data>>> move_to_vector() override
  {
    std::vector<std::pair<std::string, std::unique_ptr<Data>>> result;
    datas_.swap(result);
    return std::move(result);
  }

  // ---------------------------------------------------------------------------
  // Non public API
  // ---------------------------------------------------------------------------

  /**
   * @brief Appends `rhs` to the end of `this` composite.
   */
  void append(heap_data_Composite&& rhs)
  {
    datas_.insert(cend(datas_), std::make_move_iterator(begin(rhs.datas_)), std::make_move_iterator(end(rhs.datas_)));
    DMITIGR_ASSERT(is_invariant_ok());
  }

protected:
  bool is_invariant_ok() override
  {
    return true;
  }

private:
  std::vector<std::pair<std::string, std::unique_ptr<Data>>> datas_;
};

} // namespace dmitigr::pgfe::detail

namespace dmitigr::pgfe {

DMITIGR_PGFE_INLINE std::unique_ptr<Composite> Composite::make()
{
  return std::make_unique<detail::heap_data_Composite>();
}

DMITIGR_PGFE_INLINE std::unique_ptr<Composite> Composite::make(std::vector<std::pair<std::string, std::unique_ptr<Data>>>&& v)
{
  return std::make_unique<detail::heap_data_Composite>(std::move(v));
}

} // namespace dmitigr::pgfe
