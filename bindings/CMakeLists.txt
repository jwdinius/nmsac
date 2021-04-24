cmake_minimum_required(VERSION 3.5)
project(bindings)

if (${BUILD_PYTHON_BINDINGS})
  add_subdirectory(python)
endif()

add_library(${PROJECT_NAME} INTERFACE)

target_link_libraries(${PROJECT_NAME}
  INTERFACE
  $<$<BOOL:${BUILD_PYTHON_BINDINGS}>:${PROJECT_NAME}::python>
)
