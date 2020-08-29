// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt or pgfe.hpp

#include "dmitigr/pgfe/compositional.hpp"

namespace dmitigr::pgfe::detail {

inline bool is_invariant_ok(const Compositional& o)
{
  const bool fields_ok = !o.has_fields() || ((o.field_count() > 0));
  const bool field_names_ok = [&o]
  {
    const std::size_t fc = o.field_count();
    for (std::size_t i = 0; i < fc; ++i)
      if (o.field_index(o.field_name(i), i) != i)
        return false;
    return true;
  }();

  return fields_ok && field_names_ok;
}

} // namespace dmitigr::pgfe::detail
