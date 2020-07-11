project(tranforms_test)

include(CTest)

find_package(nlohmann_json 3.7.0 REQUIRED)

set(main_src "${CMAKE_CURRENT_SOURCE_DIR}/../main.cpp")

add_executable(svd_test ${main_src} svd_test.cpp)

# Create namespaced alias
add_executable(${PROJECT_NAME}::svd_test ALIAS svd_test)
add_test(${PROJECT_NAME}::svd_test svd_test)

target_include_directories(svd_test
    PRIVATE
    ${TEST_DATA_INCLUDE}

    PUBLIC

    INTERFACE
)

target_link_libraries(svd_test
    PRIVATE
    ${ARMADILLO_LIBRARIES}
    nlohmann_json::nlohmann_json
    transforms::svd
    gtest_main

    PUBLIC

    INTERFACE
)

target_compile_features(svd_test
    PRIVATE
        cxx_std_17
)

add_executable(icp_test ${main_src} icp_test.cpp)

# Create namespaced alias
add_executable(${PROJECT_NAME}::icp_test ALIAS icp_test)
add_test(${PROJECT_NAME}::icp_test icp_test)

target_include_directories(icp_test
    PRIVATE
    ${TEST_DATA_INCLUDE}

    PUBLIC

    INTERFACE
)

target_link_libraries(icp_test
    PRIVATE
    ${ARMADILLO_LIBRARIES}
    nlohmann_json::nlohmann_json
    transforms::svd
    transforms::icp
    gtest_main

    PUBLIC

    INTERFACE
)

target_compile_options(icp_test
    PRIVATE

    PUBLIC

    INTERFACE
)

target_compile_features(icp_test
    PRIVATE
        cxx_std_17
)
