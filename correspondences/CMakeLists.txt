cmake_minimum_required(VERSION 3.5)
project(correspondences)

add_subdirectory(common)

if (${BUILD_MC})
  add_subdirectory(graph)
  add_subdirectory(mc)
endif()

if (${BUILD_QAP})
  add_subdirectory(qap)
endif()

add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME}
  INTERFACE
    ${PROJECT_NAME}::common
    $<$<BOOL:${BUILD_MC}>:${PROJECT_NAME}::graph>
    $<$<BOOL:${BUILD_MC}>:${PROJECT_NAME}::mc>
    $<$<BOOL:${BUILD_QAP}>:${PROJECT_NAME}::qap>
)
