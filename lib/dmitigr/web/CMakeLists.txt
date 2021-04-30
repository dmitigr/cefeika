# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(web 0 1 "Web stuff")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_web_headers
  mulf.hpp
  url.hpp
  v1.hpp
  types_fwd.hpp
  )

set(dmitigr_web_implementations
  )

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(web)
