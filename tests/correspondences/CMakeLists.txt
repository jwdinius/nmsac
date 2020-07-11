project(correspondences_test)

include(CTest)

find_package(OpenMP)
find_package(nlohmann_json 3.7.0 REQUIRED)

set(main_src "${CMAKE_CURRENT_SOURCE_DIR}/../main.cpp")

add_executable(common_test ${main_src} utilities_test.cpp)

# Create namespaced alias
add_executable(${PROJECT_NAME}::common_test ALIAS common_test)
add_test(${PROJECT_NAME}::common_test common_test)

target_include_directories(common_test
    PRIVATE
    ${TEST_DATA_INCLUDE}

    PUBLIC

    INTERFACE
)

target_link_libraries(common_test
    PRIVATE
    ${ARMADILLO_LIBRARIES}
    nlohmann_json::nlohmann_json
    correspondences::common
    gtest_main

    PUBLIC

    INTERFACE
)

target_compile_features(common_test
    PRIVATE
        cxx_std_17
)

add_executable(qap_test ${main_src} qap_test.cpp)

# Create namespaced alias
add_executable(${PROJECT_NAME}::qap_test ALIAS qap_test)
add_test(${PROJECT_NAME}::qap_test qap_test)

target_include_directories(qap_test
    PRIVATE
    ${TEST_DATA_INCLUDE}

    PUBLIC

    INTERFACE
)

target_link_libraries(qap_test
    PRIVATE
    ${ARMADILLO_LIBRARIES}
    nlohmann_json::nlohmann_json
    correspondences::qap
    gtest_main

    PUBLIC

    INTERFACE
)

target_compile_features(qap_test
    PRIVATE
        cxx_std_17
)
