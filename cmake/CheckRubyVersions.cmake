#.rst:
# CheckRubyVersions
# -----------------
#
# Provides a macro to check if _current_ (not system) ruby header and
# lib dirs can be found
#
# .. command:: FIND_RUBY_DIRS
#
#   ::
#
#     FIND_RUBY_DIRS()

function(__read_rbconfig var)
  if(NOT CMAKE_REQUIRED_QUIET)
    message(STATUS "Reading RbConfig::CONFIG")
  endif()

  set(__${var})
  execute_process(
    COMMAND ruby -r mkmf -e "print RbConfig::CONFIG"
    OUTPUT_VARIABLE output)

  if(NOT "${output}" STREQUAL "")
    if(NOT CMAKE_REQUIRED_QUIET)
      message(STATUS "Reading RbConfig::CONFIG - done")
    endif()
    set(${var} "${output}" CACHE INTERNAL "FIND_RUBY_DIRS")
  else()
    if(NOT CMAKE_REQUIRED_QUIET)
      message(STATUS "Reading RbConfig::CONFIG - failed")
    endif()
    set(${var} "" CACHE INTERNAL "FIND_RUBY_DIRS failed")
  endif()
endfunction()

function(__match_rb_config_opt opt var)
  if(NOT DEFINED RB_CONFIG)
    __read_rbconfig(RB_CONFIG)
  endif()
  if (${RB_CONFIG} MATCHES "\"${opt}\"=>\"([^\"]+)\",")
    set(HAS_${var} 1 CACHE INTERNAL "Found RbConfig ${opt}")
  else()
    set(HAS_${var} "" CACHE INTERNAL "Found RbConfig ${opt}")
  endif()

  if (HAS_${var})
    if(NOT CMAKE_REQUIRED_QUIET)
      message(STATUS "Looking for ${var} - ${CMAKE_MATCH_1}")
    endif()
    set(${var} ${CMAKE_MATCH_1} CACHE INTERNAL "FIND_RUBY_DIRS ${opt}")
  else()
    if(NOT CMAKE_REQUIRED_QUIET)
      message(STATUS "Looking for ${var} - not found")
    endif()
    set(${var} "" CACHE INTERNAL "FIND_RUBY_DIRS ${opt} missing")
  endif()
endfunction()

macro(FIND_RUBY_DIRS)
  __match_rb_config_opt(rubyhdrdir     RUBY_HEADER_DIR)
  __match_rb_config_opt(rubyarchhdrdir RUBY_ARCH_HEADER_DIR)
  __match_rb_config_opt(rubylibdir     RUBY_LIBRARY_DIR)
  __match_rb_config_opt(LIBRUBY        RUBY_LIBRARY)
  __match_rb_config_opt(prefix         RUBY_PREFIX)
endmacro()
