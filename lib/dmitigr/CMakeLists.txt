# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

dmitigr_append_cppfs(dmitigr_filesystem_target_link_libraries_interface)
if(UNIX)
  list(APPEND dmitigr_concur_target_link_libraries_interface pthread)
endif()

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

foreach(lib concur filesystem)
  dmitigr_cefeika_propagate_library_settings(${lib})
endforeach()
