find_package(OpenMP)
find_package(Armadillo REQUIRED)
find_package(MLPACK REQUIRED)
find_package(nlohmann_json 3.7.0 REQUIRED)

set(target nmsac)

add_library(${target}
  src/helper.cpp
  src/registration.cpp
  src/sampling.cpp
  src/main.cpp
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

# TODO(jwd) - add generator expression for includes below
target_link_libraries(${target}
    PRIVATE
    ${ARMADILLO_LIBRARIES}
    ${MLPACK_LIBRARIES}
    correspondences::qap
    transforms

    PUBLIC
    correspondences::common
    nlohmann_json::nlohmann_json
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
