find_package(OpenMP)
find_package(Armadillo REQUIRED)
find_package(MLPACK REQUIRED)
find_package(nlohmann_json 3.7.0 REQUIRED)
find_package(pybind11 REQUIRED)
find_package(carma REQUIRED)

set(target nmsac)

add_library(${target} SHARED
  src/helper.cpp
  src/registration.cpp
  src/sampling.cpp
  src/main.cpp
)

# Create namespaced alias
add_library(${PROJECT_NAME}::${target} ALIAS ${target})

target_include_directories(${target}
    PRIVATE
    ${carma_INCLUDE_DIR}

    PUBLIC
    ${ARMADILLO_INCLUDE_DIRS}
    ${MLPACK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include

    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

target_link_libraries(${target}
    PRIVATE
    transforms

    PUBLIC
    ${ARMADILLO_LIBRARIES}
    ${MLPACK_LIBRARIES}
    correspondences
    nlohmann_json::nlohmann_json
    $<$<BOOL:${OpenMP_FOUND}>:OpenMP::OpenMP_CXX>

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
