project(nmsac_test)

include(CTest)

find_package(nlohmann_json 3.7.0 REQUIRED)

set(main_src "${CMAKE_CURRENT_SOURCE_DIR}/../main.cpp")

add_executable(main_test ${main_src} main_test.cpp)

# Create namespaced alias
add_executable(${PROJECT_NAME}::main_test ALIAS main_test)
add_test(${PROJECT_NAME}::main_test main_test)

target_include_directories(main_test
    PRIVATE
    ${TEST_DATA_INCLUDE}

    PUBLIC
    ${ARMADILLO_INCLUDE_DIRS}

    INTERFACE
)

target_link_libraries(main_test
    PRIVATE
    ${ARMADILLO_LIBRARIES}
    nlohmann_json::nlohmann_json
    gtest_main

    PUBLIC
    nmsac

    INTERFACE
)

target_compile_features(main_test
    PRIVATE
        cxx_std_17
)

add_executable(helper_test ${main_src} helper_test.cpp)

# Create namespaced alias
add_executable(${PROJECT_NAME}::helper_test ALIAS helper_test)
add_test(${PROJECT_NAME}::helper_test helper_test)

target_include_directories(helper_test
    PRIVATE
    ${TEST_DATA_INCLUDE}

    PUBLIC

    INTERFACE
)

target_link_libraries(helper_test
    PRIVATE
    ${ARMADILLO_LIBRARIES}
    nlohmann_json::nlohmann_json
    gtest_main

    PUBLIC
    transforms::icp
    nmsac

    INTERFACE
)

target_compile_features(helper_test
    PRIVATE
        cxx_std_17
)

add_executable(sampling_test ${main_src} sampling_test.cpp)

# Create namespaced alias
add_executable(${PROJECT_NAME}::sampling_test ALIAS sampling_test)
add_test(${PROJECT_NAME}::sampling_test sampling_test)

target_include_directories(sampling_test
    PRIVATE
    ${TEST_DATA_INCLUDE}

    PUBLIC

    INTERFACE
)

target_link_libraries(sampling_test
    PRIVATE
    ${ARMADILLO_LIBRARIES}
    nlohmann_json::nlohmann_json
    gtest_main

    PUBLIC
    nmsac

    INTERFACE
)

target_compile_features(sampling_test
    PRIVATE
        cxx_std_17
)


