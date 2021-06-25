// -*- C++ -*-
// Copyright (C) 2021 Dmitry Igrishin
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
// Dmitry Igrishin
// dmitigr@gmail.com

#ifndef DMITIGR_QUE_FIFO_STRING_HPP
#define DMITIGR_QUE_FIFO_STRING_HPP

#include <algorithm>
#include <string>
#include <utility>

namespace dmitigr::que {

/**
 * A container adapter that gives the functionality of a FIFO structure.
 *
 * This class can be used as the underlying container of `std::queue`.
 */
template<class CharT, class Traits = std::char_traits<CharT>,
  class Allocator = std::allocator<CharT>>
class Basic_fifo_string final {
public:
  using Underlying_type = std::basic_string<CharT, Traits, Allocator>;
  using value_type = typename Underlying_type::value_type;
  using reference = typename Underlying_type::reference;
  using const_reference = typename Underlying_type::const_reference;
  using size_type = typename Underlying_type::size_type;

  template<typename ... Types>
  constexpr explicit Basic_fifo_string(Types&& ... args)
    : data_(std::forward<Types>(args)...)
  {}

  constexpr std::basic_string_view<CharT, Traits> view() const noexcept
  {
    return {data(), size()};
  }

  constexpr const value_type* data() const noexcept
  {
    return data_.data() + offset_;
  }

  constexpr value_type* data() noexcept
  {
    return const_cast<value_type*>(static_cast<const decltype(this)>(this)->data());
  }

  constexpr const value_type& back() const noexcept
  {
    return data_.back();
  }

  constexpr value_type& back() noexcept
  {
    return const_cast<value_type&>(static_cast<const decltype(this)>(this)->back());
  }

  constexpr const value_type& front() const noexcept
  {
    return *(data_.begin() + offset_);
  }

  constexpr value_type& front() noexcept
  {
    return const_cast<value_type&>(static_cast<const decltype(this)>(this)->front());
  }

  constexpr void push_back(const value_type value)
  {
    data_.push_back(value);
  }

  constexpr void emplace_back(const value_type value)
  {
    push_back(value);
  }

  constexpr void pop_front() noexcept
  {
    offset_ = std::min(offset_ + 1, data_.size());
  }

  constexpr void unpop_front() noexcept
  {
    if (offset_) --offset_;
  }

  constexpr void unpop_all() noexcept
  {
    offset_ = 0;
  }

  constexpr size_type size() const noexcept
  {
    return data_.size() - offset_;
  }

  constexpr bool empty() const noexcept
  {
    return !size();
  }

  constexpr void clear() noexcept
  {
    data_.clear();
    offset_ = 0;
  }

  constexpr void swap(Basic_fifo_string& other) noexcept
  {
    using std::swap;
    swap(data_, other.data_);
    swap(offset_, other.offset_);
  }

private:
  Underlying_type data_;
  size_type offset_{};
};

/// Swaps `lhs` and `rhs`.
template<class CharT, class Traits = std::char_traits<CharT>,
  class Allocator = std::allocator<CharT>>
void swap(Basic_fifo_string<CharT, Traits, Allocator>& lhs,
  Basic_fifo_string<CharT, Traits, Allocator>& rhs) noexcept
{
  lhs.swap(rhs);
}

using Fifo_string = Basic_fifo_string<char>;

} // namespace dmitigr::que

#endif  // DMITIGR_QUE_FIFO_STRING_HPP
