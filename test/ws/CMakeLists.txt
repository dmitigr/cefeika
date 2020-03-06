# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_ws_tests echo echo-threads threads)
set(dmitigr_ws_tests_target_link_libraries dmitigr::rng dmitigr::thread)
dmitigr_propagate_tests_settings(ws)
