find_program(RUBY "ruby")

if(RUBY)
  set(RUBYTHON_GEMSPEC_IN "${CMAKE_CURRENT_SOURCE_DIR}/rubython.gemspec.in")
  set(RUBYTHON_GEMSPEC    "${CMAKE_CURRENT_BINARY_DIR}/rubython.gemspec")
  configure_file(${RUBYTHON_GEMSPEC_IN} ${RUBYTHON_GEMSPEC})
else(RUBY)
  message(FATAL_ERROR "Could not find Ruby. Bailing out.")
endif(RUBY)

file(GLOB_RECURSE RUBY_SOURCES
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "lib/**/*.rb")

add_custom_target(rubython_ruby_lib ALL
  COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/lib ${CMAKE_CURRENT_BINARY_DIR}/.
  COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/rubython_ext.bundle ${CMAKE_CURRENT_BINARY_DIR}/lib/rubython/.
  DEPENDS ${RUBY_SOURCES}
  DEPENDS rubython_ruby_ext)
add_custom_target(rubython_ruby_gem ALL
  COMMAND gem build ${RUBYTHON_GEMSPEC}
  DEPENDS rubython_ruby_lib)

install(CODE "execute_process(WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND gem install rubython)")

add_subdirectory(spec)
