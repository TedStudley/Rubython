set(SOURCES
  rubython_ext

  rb_Rubython.c
  rb_PyContext.c
  rb_PyTypes.c
  rb_PyObject.c
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
COMPUTE_LIB_OFFSET(${RUBY_LIBRARY_PATH} origarg.1 ORIGARG_OFFSET)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/config.h")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_C_COMPILER /usr/local/bin/gcc)
set(CMAKE_LINKER /usr/local/bin/gcc)

set(CFLAGS_SHARED "-dynamic -fno-common -fno-strict-aliasing -arch x86_64 -pipe -Wall -Wstrict-prototypes -Wshorten-64-to-32 -Wdeprecated -Wno-unused-function -Wno-unused-variable")
set(LDFLAGS_SHARED "-dynamic -bundle -fPIC -Bstatic -fstack-protector -Wl,-undefined,dynamic_lookup -Wl,-multiply_defined,suppress -arch x86_64")

add_library(${PROJECT_NAME}_ruby_ext MODULE ${SOURCES})
target_link_libraries(${PROJECT_NAME}_ruby_ext z pthread dl objc python)
add_definitions(-D_XOPEN_SOURCE -D_DARWIN_C_SOURCE -D_DARWIN_UNLIMITED_SELECT -D_REENTRANT)
set_target_properties(${PROJECT_NAME}_ruby_ext PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}_ext
    COMPILE_FLAGS "-O3 -fPIC -mmacosx-version-min=10.7 ${CFLAGS_SHARED}"
    LINK_FLAGS "-L. ${LDFLAGS_SHARED}"
    PREFIX "" SUFFIX .bundle
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/ruby/lib/rubython/"
    EXCLUDE_FROM_ALL TRUE)

set(CFLAGS "-fwrapv -Os -g ${CFLAGS_SHARED}")
set(LDFLAGS "-undefined dynamic_lookup -Wl,-F. -Wl,-L,${RUBY_LIBRARY_DIR} -Wl,-rpath,${RUBY_LIBRARY_DIR} ${LDFLAGS_SHARED}")
link_directories(${RUBY_LIBRARY_DIR})
add_library(${PROJECT_NAME}_python_ext MODULE ${SOURCES})
target_link_libraries(${PROJECT_NAME}_python_ext pthread dl objc ruby)
add_definitions(-DENABLE_DTRACE -DMACOSX -DNDEBUG)
set_target_properties(${PROJECT_NAME}_python_ext PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}_ext
    PREFIX "" SUFFIX .so
    COMPILE_FLAGS ${CFLAGS}
    LINK_FLAGS ${LDFLAGS}
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python/rubython/"
    EXCLUDE_FROM_ALL TRUE)
