# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_wscl_tests hello)
set(dmitigr_wscl_tests_target_link_libraries dmitigr::wscl)

if(WIN32)
  set(dmitigr_wscl_tests_target_compile_definitions WIN32_LEAN_AND_MEAN)
endif()

dmitigr_propagate_tests_settings(wscl)
