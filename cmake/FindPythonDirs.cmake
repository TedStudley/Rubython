#.rst
# FindPythonDirs
#
# provides a macro to chekc if _current_ (not system) python header and
# lib dirs can be found
#
# .. command:: FIND_PYTHON_DIRS
#
#   ::
#
#     FIND_PYTHON_DIRS()

function(__read_sysconfig var)
  if(NOT CMAKE_REQUIRED_QUIET)
    message(STATUS "Reading distutils.sysconfig")
  endif()

  set(__${var})
  execute_process(
    COMMAND python -c "import distutils.sysconfig as c; print(c.get_config_vars())"
    OUTPUT_VARIABLE output)

  if(NOT "${output}" STREQUAL "")
    if(NOT CMAKE_REQUIRED_QUIET)
      message(STATUS "Reading distutils.sysconfig - done")
    endif()
    set(${var} "${output}" CACHE INTERNAL "FIND_PYTHON_DIRS")
  else()
    if(NOT CMAKE_REQUIRED_QUIET)
      message(STATUS "Reading distutils.sysconfig - failed")
    endif()
    set(${var} "" CACHE INTERNAL "FIND_PYTHON_DIRS failed")
  endif()
endfunction()

function(__match_sysconfig_opt opt var)
  if(NOT DEFINED SYSCONFIG)
    __read_sysconfig(SYSCONFIG)
  endif()
  if(${SYSCONFIG} MATCHES "'${opt}': '([^']+)',")
    set(HAS_${var} 1 CACHE INTERNAL "Found sysconfig ${opt}")
  else()
    set(HAS_${var} 0 CACHE INTERNAL "Found sysconfig ${opt}")
  endif()

  if (HAS_${var})
    if(NOT CMAKE_REQUIRED_QUIET)
      message(STATUS "Looking for ${var} - ${CMAKE_MATCH_1}")
    endif()
    set(${var} ${CMAKE_MATCH_1} CACHE INTERNAL "FIND_PYTHON_DIRS ${opt}")
  else()
    if(NOT CMAKE_REQUIRED_QUIET)
      message(STATUS "Looking for ${var} - not found")
    endif()
    set(${var} "" CACHE INTERNAL "FIND_RUBY_DIRS ${opt} missing")
  endif()
endfunction()

macro(FIND_PYTHON_DIRS)
  __match_sysconfig_opt(prefix           PYTHON_PREFIX)
  __match_sysconfig_opt(LIBDIR           PYTHON_LIBRARY_DIR)
  __match_sysconfig_opt(PYTHON_FOR_REGEN PYTHON_LIBNAME)
  __match_sysconfig_opt(INCLUDEPY        PYTHON_HEADER_DIR)
endmacro()
