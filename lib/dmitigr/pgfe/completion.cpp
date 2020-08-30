// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#include "dmitigr/pgfe/completion.hpp"
#include <dmitigr/base/debug.hpp>

namespace dmitigr::pgfe {

DMITIGR_PGFE_INLINE Completion::Completion(const std::string_view tag)
{
  constexpr char space{' '};
  auto space_before_word_pos = tag.find_last_of(space);
  if (space_before_word_pos != std::string_view::npos) {
    auto end_word_pos = tag.size() - 1;
    while (space_before_word_pos != std::string_view::npos) {
      /*
       * The tag can include affected row count as the last word. We'll try to
       * convert each word of the tag to a number. All numbers except the last
       * one (i.e. affected row count) must be ignored.
       */
      const auto word_size = end_word_pos - space_before_word_pos;
      const std::string word{tag.substr(space_before_word_pos + 1, word_size)};
      try {
        const auto count = std::stol(word);
        if (!affected_row_count_)
          affected_row_count_ = count;
      } catch (std::invalid_argument&) {
        // The word is not a number.
        break;
      } catch (std::out_of_range&) {
        // Enormous number value.
        throw;
      }
      end_word_pos = space_before_word_pos - 1;
      space_before_word_pos = tag.find_last_of(space, end_word_pos);
    }
    operation_name_ = tag.substr(0, end_word_pos + 1);
  } else
    operation_name_ = tag;

  DMITIGR_ASSERT(is_invariant_ok());
}

} // namespace dmitigr::pgfe
