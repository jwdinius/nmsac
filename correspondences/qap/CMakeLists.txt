list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Armadillo REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(nlohmann_json 3.7.0 REQUIRED)
find_package(CppAD REQUIRED)
find_package(IPOPT REQUIRED)

set(target qap)

add_library(${target} SHARED
  src/qap.cpp
)

# Create namespaced alias
add_library(${PROJECT_NAME}::${target} ALIAS ${target})

target_include_directories(${target}
    PRIVATE
    ${CPPAD_INCLUDE_DIRS}
    ${IPOPT_INCLUDE_DIRS}

    PUBLIC
    ${ARMADILLO_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include

    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

target_link_libraries(${target}
    PRIVATE
    ${CPPAD_LIBRARIES}
    ${IPOPT_LIBRARIES} 

    PUBLIC
    ${ARMADILLO_LIBRARIES}
    correspondences::common
    nlohmann_json::nlohmann_json

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
