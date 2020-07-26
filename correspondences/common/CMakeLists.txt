list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_PREFIX_PATH $ENV{ORTOOLS_ROOT})

find_package(Armadillo REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(ortools REQUIRED)

set(target common)

add_library(${target} SHARED
  src/utilities.cpp
)

# Create namespaced alias
add_library(${PROJECT_NAME}::${target} ALIAS ${target})

target_include_directories(${target}
    PRIVATE
    ${ORTOOLS_INCLUDE_DIRS}

    PUBLIC
    ${ARMADILLO_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include

    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

target_link_libraries(${target}
    PRIVATE
    ${Boost_LIBRARIES}
    ${ORTOOLS_LIBRARIES}

    PUBLIC
    ${ARMADILLO_LIBRARIES}

    INTERFACE
)

target_compile_options(${target}
    PRIVATE
    -DUSE_GLOP

    PUBLIC

    INTERFACE
)
