# -*- cmake -*-
# Copyright (C) 2021 Dmitry Igrishin
#
# This software is provided 'as-is', without any express or implied
# warranty. In no event will the authors be held liable for any damages
# arising from the use of this software.
#
# Permission is granted to anyone to use this software for any purpose,
# including commercial applications, and to alter it and redistribute it
# freely, subject to the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not
#    claim that you wrote the original software. If you use this software
#    in a product, an acknowledgment in the product documentation would be
#    appreciated but is not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.

function(dmitigr_cefeika_load_with_deps component)
  # Loading the component's dependencies
  foreach(dep ${dmitigr_cefeika_${component}_deps})
    dmitigr_cefeika_load_with_deps(${dep})
  endforeach()

  # Loading the component
  string(REGEX MATCH "(shared|static|interface)$" suffix "${component}")
  if(NOT suffix)
    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/dmitigr_${component}_shared-config.cmake)
      set(suffix "shared")
    elseif(EXISTS ${CMAKE_CURRENT_LIST_DIR}/dmitigr_${component}_static-config.cmake)
      set(suffix "static")
    elseif(EXISTS ${CMAKE_CURRENT_LIST_DIR}/dmitigr_${component}_interface-config.cmake)
      set(suffix "interface")
    endif()
    set(component "${component}_${suffix}")
  endif()
  set(config_file "${CMAKE_CURRENT_LIST_DIR}/dmitigr_${component}-config.cmake")
  if(EXISTS ${config_file})
    include(${config_file})
  else()
    message(FATAL_ERROR "No configuration files of the Dmitigr Cefeika ${component} component found")
  endif()
endfunction()

# ------------------------------------------------------------------------------

include(${CMAKE_CURRENT_LIST_DIR}/dmitigr_cefeika_libraries.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/dmitigr_cefeika_libraries_all.cmake)

if(NOT dmitigr_cefeika_FIND_COMPONENTS)
  set(dmitigr_cefeika_FIND_COMPONENTS ${dmitigr_cefeika_libraries})
endif()

foreach(component ${dmitigr_cefeika_FIND_COMPONENTS})
  dmitigr_cefeika_load_with_deps(${component})
endforeach()
