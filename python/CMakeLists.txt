find_program(PYTHON "python")

if (PYTHON)
  set(SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
  set(SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
  set(DEPS        "${CMAKE_CURRENT_SOURCE_DIR}/rubython/__init__.py")
  set(OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/rubython")

  configure_file(${SETUP_PY_IN} ${SETUP_PY})
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/rubython/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/rubython/)

  add_custom_command(OUTPUT ${OUTPUT}
    COMMAND ${PYTHON} ${SETUP_PY} build
    COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT}_python
    DEPENDS ${DEPS}
    DEPENDS ${PROJECT_NAME}_python_ext)
  add_custom_target(rubython_python_module ALL
    DEPENDS ${OUTPUT}
    DEPENDS "${PROJECT_NAME}_python_ext")

  install(CODE "file(COPY ${CMAKE_CURRENT_BINARY_DIR}/rubython/ DESTINATION ${CMAKE_SOURCE_DIR}/rubython/)")
  install(CODE "execute_process(COMMAND ${PYTHON} ${SETUP_PY} install --prefix=${CMAKE_INSTALL_PREFIX})")
endif()
