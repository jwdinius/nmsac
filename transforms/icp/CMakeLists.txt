find_package(OpenMP)
find_package(Armadillo REQUIRED)
find_package(MLPACK REQUIRED)

set(target icp)

add_library(${target}
  src/icp.cpp
)

# Create namespaced alias
add_library(${PROJECT_NAME}::${target} ALIAS ${target})

target_include_directories(${target}
    PRIVATE
    ${MLPACK_INCLUDE_DIRS}

    PUBLIC
    ${ARMADILLO_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include

    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

target_link_libraries(${target}
    PRIVATE
    ${ARMADILLO_LIBRARIES}
    ${MLPACK_LIBRARIES}
    ${PROJECT_NAME}::svd

    PUBLIC
    OpenMP::OpenMP_CXX

    INTERFACE
)

target_compile_options(${target}
    PRIVATE

    PUBLIC

    INTERFACE
)

target_compile_features(${target}
    PUBLIC
        cxx_std_17
)
