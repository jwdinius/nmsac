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

add_executable(graph_test ${main_src} graph_test.cpp)

# Create namespaced alias
add_executable(${PROJECT_NAME}::graph_test ALIAS graph_test)
add_test(${PROJECT_NAME}::graph_test graph_test)

target_include_directories(graph_test
    PRIVATE

    PUBLIC

    INTERFACE
)

target_link_libraries(graph_test
    PRIVATE
    ${ARMADILLO_LIBRARIES}
    correspondences::common
    correspondences::graph
    gtest_main

    PUBLIC

    INTERFACE
)

target_compile_features(graph_test
    PRIVATE
        cxx_std_17
)

add_executable(mc_test ${main_src} mc_test.cpp)

# Create namespaced alias
add_executable(${PROJECT_NAME}::mc_test ALIAS mc_test)
add_test(${PROJECT_NAME}::mc_test mc_test)

target_include_directories(mc_test
    PRIVATE
    ${TEST_DATA_INCLUDE}

    PUBLIC

    INTERFACE
)

target_link_libraries(mc_test
    PRIVATE
    ${ARMADILLO_LIBRARIES}
    correspondences::common
    correspondences::graph
    correspondences::mc
    gtest_main

    PUBLIC

    INTERFACE
)

target_compile_features(mc_test
    PRIVATE
        cxx_std_17
)

if (BUILD_QAP)
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
endif()
