# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

#set(dmitigr_ws_tests echo echo-threads http threads)
set(dmitigr_ws_tests http)
set(dmitigr_ws_tests_target_link_libraries dmitigr::mp dmitigr::rng)

if(WIN32)
  set(dmitigr_ws_tests_target_compile_definitions WIN32_LEAN_AND_MEAN)
endif()

dmitigr_propagate_tests_settings(ws)
