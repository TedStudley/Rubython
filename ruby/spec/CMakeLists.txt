message(STATUS ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE RUBY_SPEC_SOURCES
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "**/*.rb")

message(STATUS ${RUBY_SPEC_SOURCES})

add_custom_target(ruby_spec
  COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/../.
  COMMAND /usr/bin/env rspec -Ilib . --color
  DEPENDS rubython_ruby_lib
  DEPENDS ${RUBY_SPEC_SOURCES}
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/..")
