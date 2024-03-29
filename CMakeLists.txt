cmake_minimum_required(VERSION 3.5)
project(nmsac)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(BUILD_TESTS "build unit tests" OFF)
option(BUILD_QAP "build correspondences::qap target" ON)
option(BUILD_MC "build correspondences::mc target" ON)
option(BUILD_PYTHON_BINDINGS "build python bindings for nmsac::main method" ON)
option(CODE_COVERAGE "build for code coverage reporting" OFF)

if (${CODE_COVERAGE})
  add_compile_options(-Wall -O0 -g -Wno-deprecated-declarations --coverage)
  add_link_options(--coverage)
else()
  # set project-wide compile options
  add_compile_options(-Wall -O3 -Wno-deprecated-declarations)
endif()

add_subdirectory(correspondences)
add_subdirectory(transforms)
add_subdirectory(nmsac)

# TODO(jwd): if/when more bindings are added, put "OR" condition here
if (${BUILD_PYTHON_BINDINGS})
  add_subdirectory(bindings)
endif()

if (${BUILD_TESTS})
  enable_testing()
  add_subdirectory(tests)
endif()
