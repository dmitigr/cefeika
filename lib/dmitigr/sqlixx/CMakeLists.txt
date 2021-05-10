# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

# ------------------------------------------------------------------------------
# Info
# ------------------------------------------------------------------------------

dmitigr_set_library_info(sqlixx 0 1 "Powerful client API for SQLite")

# ------------------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------------------

set(dmitigr_sqlixx_headers
  sqlixx.hpp
  )

set(dmitigr_sqlixx_implementations
  )

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------

find_package(SQLite3 REQUIRED)
list(APPEND dmitigr_sqlixx_target_include_directories_interface "${SQLite3_INCLUDE_DIRS}")
list(APPEND dmitigr_sqlixx_target_link_libraries_interface ${SQLite3_LIBRARIES})

if (UNIX)
  list(APPEND dmitigr_sqlixx_target_link_libraries_interface pthread)
endif()

# ------------------------------------------------------------------------------
# Variables propagation
# ------------------------------------------------------------------------------

dmitigr_propagate_library_settings(sqlixx)
