cmake_minimum_required(VERSION 3.5)
project(correspondences)

add_subdirectory(common)
add_subdirectory(qap)

add_library(${PROJECT_NAME} INTERFACE)

target_link_libraries(${PROJECT_NAME}
  INTERFACE
    ${PROJECT_NAME}::common
    ${PROJECT_NAME}::qap
)
