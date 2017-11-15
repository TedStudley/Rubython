set(SOURCES
  rubython_ext

  rb_Rubython.c
  rb_PyContext.c
  rb_PyTypes.c
  rb_PyObject.c
  rb_PyDict.c
  rb_PyType.c
  rb_PyString.c
  rb_PyFunction.c
  rb_PyCFunction.c

  py_Rubython.c
  py_RbContext.c
  py_RbTypes.c
  py_RbObject.c
  py_RbArray.c
  py_RbHash.c
  py_RbString.c

  ruby_shims.c

  utility.c
  conversion.c)

include(${PROJECT_SOURCE_DIR}/cmake/CheckRubyVersions.cmake)
FIND_RUBY_DIRS()
include_directories(${RUBY_HEADER_DIR} ${RUBY_ARCH_HEADER_DIR})

include(${PROJECT_SOURCE_DIR}/cmake/ComputeLibOffsets.cmake)
set(RUBY_LIBRARY_DIR ${RUBY_PREFIX}/lib)
set(RUBY_LIBRARY_PATH ${RUBY_LIBRARY_DIR}/${RUBY_LIBRARY})
COMPUTE_LIB_OFFSET(${RUBY_LIBRARY_PATH} rb_ary_new RB_ARY_NEW_OFFSET)
COMPUTE_LIB_OFFSET(${RUBY_LIBRARY_PATH} ruby_current_vm RUBY_CURRENT_VM_OFFSET)
COMPUTE_LIB_OFFSET(${RUBY_LIBRARY_PATH} ruby_current_thread RUBY_CURRENT_THREAD_OFFSET)
COMPUTE_LIB_OFFSET(${RUBY_LIBRARY_PATH} Init_prelude INIT_PRELUDE_OFFSET)
COMPUTE_LIB_OFFSET(${RUBY_LIBRARY_PATH} Init_enc INIT_ENC_OFFSET)
COMPUTE_LIB_OFFSET(${RUBY_LIBRARY_PATH} Init_ext INIT_EXT_OFFSET)
COMPUTE_LIB_OFFSET(${RUBY_LIBRARY_PATH} origarg ORIGARG_OFFSET)

COMPUTE_LIB_OFFSET(${RUBY_LIBRARY_PATH} initialized RUBY_SETUP_INITIALIZED_OFFSET)
COMPUTE_LIB_OFFSET(${RUBY_LIBRARY_PATH} frozen_strings FROZEN_STRINGS_OFFSET)
COMPUTE_LIB_OFFSET(${RUBY_LIBRARY_PATH} global_method_cache GLOBAL_METHOD_CACHE_OFFSET)
COMPUTE_LIB_OFFSET(${RUBY_LIBRARY_PATH} ruby_vm_global_method_state RUBY_VM_GLOBAL_METHOD_STATE_OFFSET)
COMPUTE_LIB_OFFSET(${RUBY_LIBRARY_PATH} ruby_vm_global_constant_state RUBY_VM_GLOBAL_CONSTANT_STATE_OFFSET)
COMPUTE_LIB_OFFSET(${RUBY_LIBRARY_PATH} ruby_vm_class_serial RUBY_VM_CLASS_SERIAL_OFFSET)
COMPUTE_LIB_OFFSET(${RUBY_LIBRARY_PATH} global_symbols GLOBAL_SYMBOLS_OFFSET)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/config.h")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CFLAGS "-dynamic -pipe -fno-common -fno-strict-aliasing -Wall -Wstrict-prototypes -Wdeprecated -Wno-unused-function -Wno-unused-variable -std=c99")
set(LDFLAGS "-dynamic -fPIC -Bstatic -fstack-protector -Wl,-undefined,dynamic_lookup")

if(APPLE)
  find_package(PythonLibs)
  if(PYTHONLIBS_FOUND)
    message(STATUS ${PYTHON_INCLUDE_DIRS})
    include_directories(${PYTHON_INCLUDE_DIRS})
  else(PYTHONLIBS_FOUND)
    message(FATAL_ERROR "Could not locate python libraries")
  endif(PYTHONLIBS_FOUND)

  # TODO: Don't hardcode these
  set(PY_SUFFIX ".so")
  set(RB_SUFFIX ".bundle")

  set(CFLAGS "${CFLAGS} -Wshorten-64-to-32 -mmacosx-version-min=10.7")
  set(LDFLAGS "${LDFLAGS} -bundle -Wl,-multiply_defined,suppress")
  set(LIBRARIES ${LIBRARIES} dl objc) # z pthread
else(APPLE)
  # TODO: Fix this
  include_directories("/usr/local/python2.7/include/python2.7")
  set(PYTHON_LIBRARIES "python2.7")

  # TODO: Don't hardcode these
  set(PY_SUFFIX ".so")
  set(RB_SUFFIX ".so")

  # TODO: ???
  set(CFLAGS "${CFLAGS}")
  set(LDFLAGS "${LDFLAGS}")
  set(LIBRARIES ${LIBRARIES})
endif(APPLE)

set(DEBUG TRUE)
if(DEBUG)
  set(CFLAGS "${CFLAGS} -O0 -g3 -fPIC")
else(DEBUG)
  set(CFLAGS "${CFLAGS} -O3")
endif(DEBUG)

add_library(${PROJECT_NAME}_ruby_ext MODULE ${SOURCES})
target_link_libraries(${PROJECT_NAME}_ruby_ext ${LIBRARIES} ${PYTHON_LIBRARIES})
set_target_properties(${PROJECT_NAME}_ruby_ext PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}_ext
    COMPILE_FLAGS "${CFLAGS}"
    LINK_FLAGS "${LDFLAGS}"
    PREFIX "" SUFFIX ${RB_SUFFIX}
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/ruby/"
    EXCLUDE_FROM_ALL TRUE)

link_directories(${RUBY_LIBRARY_DIR})
add_library(${PROJECT_NAME}_python_ext MODULE ${SOURCES})
target_link_libraries(${PROJECT_NAME}_python_ext ${LIBRARIES} ${RUBY_LIBRARY_PATH})
set_target_properties(${PROJECT_NAME}_python_ext PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}_ext
    PREFIX "" SUFFIX ${PY_SUFFIX}
    COMPILE_FLAGS ${CFLAGS}
    LINK_FLAGS ${LDFLAGS}
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python/"
    EXCLUDE_FROM_ALL TRUE)
