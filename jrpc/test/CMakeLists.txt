# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_jrpc_tests request response)
set(dmitigr_jrpc_tests_target_link_libraries dmitigr_math dmitigr_testo)
dmitigr_cefeika_propagate_tests_settings(jrpc)
