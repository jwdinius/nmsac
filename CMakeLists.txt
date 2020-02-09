cmake_minimum_required(VERSION 2.8.10)
project(nmsac CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
find_package(MLPACK REQUIRED)
find_package(Armadillo REQUIRED)
find_package(Boost COMPONENTS system filesystem unit_test_framework python REQUIRED)
find_package(PythonLibs 2.7 REQUIRED)
find_package(nlohmann_json 3.7.0 REQUIRED)
find_package(Register REQUIRED)
# For Boost testing framework (will have no effect on non-testing executables)
add_definitions(-DBOOST_TEST_DYN_LINK)

include_directories(
    ${ARMADILLO_INCLUDE_DIRS}
    ${MLPACK_INCLUDE_DIRS}
    ${NLOHMANN_JSON_INCLUDE_INSTALL_DIR}
    ${PYTHON_INCLUDE_DIR}
    ${Register_INCLUDE}
    "include"
)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")

add_library(nmsac SHARED src/sampling-utils.cpp
    src/helper-utils.cpp
    src/nonmin-reg-utils.cpp
    src/main.cpp)
target_link_libraries(nmsac ${ARMADILLO_LIBRARIES}
    ${MLPACK_LIBRARIES}
    ${Register_LIBRARY})

add_library(pynmsac SHARED src/python-wrapper.cpp)
set_target_properties(pynmsac PROPERTIES PREFIX "")
target_link_libraries(pynmsac ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} nmsac)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/__init__.py "__all__ = ['pynmsac']")

# Set up installation targets
install(DIRECTORY include/nmsac DESTINATION include)
install(TARGETS nmsac DESTINATION lib)
install(TARGETS pynmsac DESTINATION lib/python2.7/dist-packages)

enable_testing()
add_executable(helper-unit tests/helper-unit.cpp)
target_link_libraries(helper-unit nmsac
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${MLPACK_LIBRARIES}
)
add_test(HelperUtilsTest helper-unit)

add_executable(sampling-unit tests/sampling-unit.cpp)
target_link_libraries(sampling-unit nmsac
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${MLPACK_LIBRARIES}
)
add_test(SamplingUtilsTest sampling-unit)

add_executable(main-unit tests/main-unit.cpp)
target_link_libraries(main-unit nmsac
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${MLPACK_LIBRARIES}
)
add_test(MainTest main-unit)
