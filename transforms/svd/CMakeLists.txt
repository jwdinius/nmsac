find_package(OpenMP)
find_package(Armadillo REQUIRED)

set(target svd)

add_library(${target} SHARED
  src/svd.cpp
)

# Create namespaced alias
add_library(${PROJECT_NAME}::${target} ALIAS ${target})

target_include_directories(${target}
    PRIVATE

    PUBLIC
    ${ARMADILLO_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include

    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

target_link_libraries(${target}
    PRIVATE
    transforms::common

    PUBLIC
    ${ARMADILLO_LIBRARIES}
    correspondences::common

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
