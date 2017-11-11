find_program(RUBY "ruby")

if (RUBY)
  set(RUBYTHON_GEMSPEC_IN "${CMAKE_CURRENT_SOURCE_DIR}/rubython.gemspec.in")
  set(RUBYTHON_GEMSPEC    "${CMAKE_CURRENT_BINARY_DIR}/rubython.gemspec")
  set(DEPS                "${CMAKE_CURRENT_SOURCE_DIR}/lib/rubython.rb")
  set(OUTPUT              "${CMAKE_CURRENT_BINARY_DIR}/lib")

  configure_file(${RUBYTHON_GEMSPEC_IN} ${RUBYTHON_GEMSPEC})
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/lib/)

  add_custom_command(OUTPUT ${OUTPUT}
    COMMAND gem build ${RUBYTHON_GEMSPEC}
    COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT}_ruby
    DEPENDS ${DEPS}
    DEPENDS "${PROJECT_NAME}_ruby_ext")
  add_custom_target(rubython_ruby_gem ALL
    DEPENDS ${OUTPUT}
    DEPENDS "${PROJECT_NAME}_ruby_ext")

  install(CODE "file(COPY ${CMAKE_CURRENT_BINARY_DIR}/lib/ DESTINATION ${CMAKE_SOURCE_DIR}/lib/)")
  install(CODE "execute_process(WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND gem install rubython)")
endif()
