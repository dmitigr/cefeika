# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(cfg 0 1 "Library to work with configuration files")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_cfg_headers
  flat.hpp
  types_fwd.hpp
  )

set(dmitigr_cfg_implementations
  )

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(cfg)
