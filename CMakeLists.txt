cmake_minimum_required(VERSION 3.5)
project(nmsac)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(BUILD_TESTS "build unit tests" ON)
option(BUILD_QAP "build correspondences::qap target" ON)
option(BUILD_PYTHON_BINDINGS "build python bindings for nmsac::main method" ON)

# set project-wide compile options
add_compile_options(-Wall -O3 -Wno-deprecated-declarations)

add_subdirectory(correspondences)
add_subdirectory(transforms)
add_subdirectory(nmsac)

# TODO(jwd): if/when more bindings are added, put "OR" condition here
if (BUILD_PYTHON_BINDINGS)
  add_subdirectory(bindings)
endif()

if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
