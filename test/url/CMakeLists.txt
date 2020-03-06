# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_url_tests qs1 qs2)
set(dmitigr_url_tests_target_link_libraries dmitigr::str)
dmitigr_propagate_tests_settings(url)
