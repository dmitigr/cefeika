# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_librarian_lib GraphicsMagick)
set(${dmitigr_librarian_lib}_include_names Magick++.h)
set(${dmitigr_librarian_lib}_library_names GraphicsMagick++)
set(${dmitigr_librarian_lib}_include_path_suffixes GraphicsMagick)

include(dmitigr_librarian)
