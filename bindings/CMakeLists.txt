cmake_minimum_required(VERSION 3.5)
project(bindings)

add_subdirectory(python)

add_library(${PROJECT_NAME} INTERFACE)

target_link_libraries(${PROJECT_NAME}
  INTERFACE
    ${PROJECT_NAME}::python
)
