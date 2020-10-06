# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_librarian_lib Ev)
set(${dmitigr_librarian_lib}_include_names ev.h)
set(${dmitigr_librarian_lib}_release_library_names ev libev)
set(${dmitigr_librarian_lib}_library_paths ${LIBEV_LIB_PREFIX})
set(${dmitigr_librarian_lib}_include_paths ${LIBEV_INCLUDE_PREFIX})

include(dmitigr_librarian)
