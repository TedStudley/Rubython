set(RUBY_SPEC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ruby")

add_custom_target(ruby_spec
      WORKING_DIRECTORY ${RUBY_SPEC_DIR}
      COMMAND /usr/bin/env rspec -I. .
      VERBATIM)
