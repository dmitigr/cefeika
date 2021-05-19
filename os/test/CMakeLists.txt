# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

if(UNIX AND NOT CMAKE_SYSTEM_NAME MATCHES MSYS|MinGW|Cygwin)
  set(dmitigr_os_tests detach proc_run)
endif()
set(dmitigr_os_tests_target_link_libraries dmitigr_testo)
dmitigr_cefeika_propagate_tests_settings(os)
