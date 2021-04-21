// -*- C++ -*-
// Copyright (C) Dmitry Igrishin
// For conditions of distribution and use, see files LICENSE.txt

#include <dmitigr/misc/assert.hpp>

int main()
{
  dmitigr_assert(true);
  if (true)
    dmitigr_assert_always(false);
}
