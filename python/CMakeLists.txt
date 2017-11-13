find_program(PYTHON "python")

if (PYTHON)
  set(RUBYTHON_SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
  set(RUBYTHON_SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
  configure_file(${RUBYTHON_SETUP_PY_IN} ${RUBYTHON_SETUP_PY})
else(PYTHON)
  message(FATAL_ERROR "Could not find Python. Bailing out.")
endif(PYTHON)

file(GLOB_RECURSE PYTHON_SOURCES
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "rubython/**/*.py")

add_custom_target(rubython_python_module ALL
  COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/rubython ${CMAKE_CURRENT_BINARY_DIR}/.
  COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/rubython_ext.so ${CMAKE_CURRENT_BINARY_DIR}/rubython/.
  DEPENDS ${PYTHON_SOURCES}
  DEPENDS rubython_python_ext)
add_custom_target(rubython_python_egg ALL
  COMMAND ${PYTHON} ${RUBYTHON_SETUP_PY} build
  DEPENDS rubython_python_module)

install(CODE "execute_process(WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND ${PYTHON} ${RUBYTHON_SETUP_PY} install --prefix=${CMAKE_INSTALL_PREFIX})")

add_subdirectory(spec)
