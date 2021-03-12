# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(img 0 1 "Image processing library")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_img_headers
  )

set(dmitigr_img_implementations
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

#
# GraphicsMagick
#

find_package(GraphicsMagick)
if (GraphicsMagick_FOUND)
  message("Found GraphicsMagick++ headers at: ${GraphicsMagick_INCLUDE_DIRS}")
  include_directories(${GraphicsMagick_INCLUDE_DIRS})
  list(APPEND dmitigr_img_headers types_fwd.hpp graphicsmagick.hpp)
  list(APPEND dmitigr_img_target_compile_definitions_public -DDMITIGR_IMG_GRAPHICSMAGICK)
  list(APPEND dmitigr_img_target_link_libraries_interface ${Suggested_GraphicsMagick_LIBRARIES})
endif()

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(img)
