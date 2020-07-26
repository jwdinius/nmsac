find_package(Armadillo REQUIRED)
find_package(pybind11 REQUIRED)
find_package(carma REQUIRED)

set(target pyNMSAC)

add_library(${target} MODULE
  src/bind_nmsac.cpp
)

# Create namespaced alias
add_library(${PROJECT_NAME}::${target} ALIAS ${target})

target_include_directories(${target}
    PRIVATE
    ${carma_INCLUDE_DIR}

    PUBLIC
    ${ARMADILLO_INCLUDE_DIRS}

    INTERFACE
)

# TODO(jwd) - add generator expression for includes below
target_link_libraries(${target}
    PRIVATE
    pybind11::module

    PUBLIC
    ${ARMADILLO_LIBRARIES}
    nmsac::nmsac

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

set_target_properties(${target}
    PROPERTIES
    PREFIX ""
    )
