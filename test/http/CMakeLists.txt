# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_http_tests basics cookie date set_cookie server)
set(dmitigr_http_tests_target_link_libraries)
dmitigr_propagate_tests_settings(http)
