# -*- cmake -*-
# Copyright (C) Dmitry Igrishin
# For conditions of distribution and use, see file LICENSE.txt

set(dmitigr_uuid_tests test)
set(dmitigr_uuid_tests_target_link_libraries dmitigr_rng dmitigr_testo)
dmitigr_cefeika_propagate_tests_settings(uuid)
