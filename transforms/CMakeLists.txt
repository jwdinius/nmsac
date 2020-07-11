cmake_minimum_required(VERSION 3.5)
project(transforms)

add_subdirectory(common)
add_subdirectory(svd)
add_subdirectory(icp)

add_library(${PROJECT_NAME} INTERFACE)

target_link_libraries(${PROJECT_NAME}
  INTERFACE
    ${PROJECT_NAME}::common
    ${PROJECT_NAME}::svd
    ${PROJECT_NAME}::icp
)
