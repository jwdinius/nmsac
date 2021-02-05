cmake_minimum_required(VERSION 3.5)
project(correspondences)

add_subdirectory(common)
add_subdirectory(graph)
add_subdirectory(mc)

if (BUILD_QAP)
  add_subdirectory(qap)
endif()

add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME}
  INTERFACE
    ${PROJECT_NAME}::common
    ${PROJECT_NAME}::graph
    ${PROJECT_NAME}::mc
    $<$<BOOL:BUILD_QAP>:${PROJECT_NAME}::qap>
)
