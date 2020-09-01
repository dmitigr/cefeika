// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#include "dmitigr/pgfe/row_info.hpp"

#include <cassert>
#include <algorithm>
#include <vector>

namespace dmitigr::pgfe {

DMITIGR_PGFE_INLINE Row_info::Row_info(detail::pq::Result&& pq_result)
  : pq_result_(std::move(pq_result))
  , shared_field_names_(make_shared_field_names(pq_result_)) // note pq_result_
{
  assert(is_invariant_ok());
}

DMITIGR_PGFE_INLINE Row_info::Row_info(detail::pq::Result&& pq_result,
  const std::shared_ptr<std::vector<std::string>>& shared_field_names)
  : pq_result_(std::move(pq_result))
  , shared_field_names_(shared_field_names)
{
  assert(is_invariant_ok());
}

std::size_t Row_info::field_index__(const std::string& name, const std::size_t offset) const
{
  assert(offset < field_count());
  const auto b = cbegin(*shared_field_names_);
  const auto e = cend(*shared_field_names_);
  const auto i = std::find(b + offset, e, name);
  return i - b;
}

std::shared_ptr<std::vector<std::string>> Row_info::make_shared_field_names(const detail::pq::Result& pq_result)
{
  assert(pq_result);
  const int fc = pq_result.field_count();
  std::vector<std::string> result;
  result.reserve(fc);
  for (int i = 0; i < fc; ++i)
    result.emplace_back(pq_result.field_name(i));

  return std::make_shared<decltype(result)>(std::move(result));
}

DMITIGR_PGFE_INLINE const std::string& Row_info::field_name(const std::size_t index) const
{
  assert(index < field_count());
  return (*shared_field_names_)[index];
}

DMITIGR_PGFE_INLINE std::size_t Row_info::field_index_throw(const std::string& name, const std::size_t offset) const
{
  const auto result = field_index__(name, offset);
  assert(result < field_count());
  return result;
}

DMITIGR_PGFE_INLINE std::uint_fast32_t Row_info::table_oid(const std::size_t index) const
{
  assert(index < field_count());
  return pq_result_.field_table_oid(static_cast<int>(index));
}

DMITIGR_PGFE_INLINE std::int_fast32_t Row_info::table_column_number(const std::size_t index) const
{
  assert(index < field_count());
  return pq_result_.field_table_column(int(index));
}

DMITIGR_PGFE_INLINE std::uint_fast32_t Row_info::type_oid(const std::size_t index) const
{
  assert(index < field_count());
  return pq_result_.field_type_oid(int(index));
}

DMITIGR_PGFE_INLINE std::int_fast32_t Row_info::type_size(const std::size_t index) const
{
  assert(index < field_count());
  return pq_result_.field_type_size(int(index));
}

DMITIGR_PGFE_INLINE std::int_fast32_t Row_info::type_modifier(const std::size_t index) const
{
  assert(index < field_count());
  return pq_result_.field_type_modifier(int(index));
}

DMITIGR_PGFE_INLINE Data_format Row_info::data_format(const std::size_t index) const
{
  assert(index < field_count());
  return pq_result_.field_format(int(index));
}

} // namespace dmitigr::pgfe
