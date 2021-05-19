# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_fcgi_tests hello hellomt largesend overload)
if(UNIX)
  set(dmitigr_fcgi_tests_target_link_libraries pthread)
endif()
list(APPEND dmitigr_fcgi_tests_target_link_libraries dmitigr_rng dmitigr_testo)
dmitigr_cefeika_propagate_tests_settings(fcgi)
