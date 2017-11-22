cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME rubython)
set(PROJECT_VERSION 0.0.1)

if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
  project(${PROJECT_NAME} VERSION ${PROJECT_VERSION})
else()
  project(${PROJECT_NAME})
endif()

set(CMAKE_INSTALL_PATH_USE_LINK_PATH TRUE)

set(DEBUG TRUE)

# Include additional required CMAKE modules.
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_SOURCE_DIR}/cmake)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "This project does not support in-source builds. Please create a 'build' directory and use `cmake ..`")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)
add_subdirectory(python)
add_subdirectory(ruby)
