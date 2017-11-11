macro(COMPUTE_LIB_OFFSET LIBPATH SYMBOL VARIABLE)
  find_program(
      OBJDUMP_EXECUTABLE
      NAMES objdump gobjdump
      PATHS ENV PATH)
  get_filename_component(LIBNAME ${LIBPATH} NAME)

  execute_process(
      COMMAND ${OBJDUMP_EXECUTABLE} --demangle -T ${LIBPATH}
      COMMAND grep "SECT"
      COMMAND grep -w "${SYMBOL}"
      COMMAND awk "{ print \"0x\"$1 }"
      COMMAND head -n1
      RESULT_VARIABLE SYMBOL_RETVAL
      OUTPUT_VARIABLE SYMBOL_ADDR
      OUTPUT_STRIP_TRAILING_WHITESPACE)

  if (SYMBOL_RETVAL)
    if(NOT CMAKE_REQUIRED_QUIET)
      message(STATUS "Locating symbol ${SYMBOL} in ${LIBNAME} - False")
    endif()
    set(${VARIABLE} ${SYMBOL_ADDR} CACHE INTERNAL "COMPUTE_LIB_OFFSET ${LIBPATH} ${SYMBOL} missing")
  else()
    if(NOT CMAKE_REQUIRED_QUIET)
      message(STATUS "Locating symbol ${SYMBOL} in ${LIBNAME} - True")
    endif()
    set(${VARIABLE} ${SYMBOL_ADDR} CACHE INTERNAL "COMPUTE_LIB_OFFSET ${LIBPATH} ${SYMBOL}")
  endif()
endmacro()
