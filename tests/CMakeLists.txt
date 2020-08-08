cmake_minimum_required(VERSION 3.5)
project(tests)

# set test parameters
set(DATA_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(FLOAT_TOL 1e-7)

# write parameters to file (can add new lines below because <glue>=newline is already included)
set(newline "\r\n")
string(JOIN newline file_content
  "inline constexpr char DATA_PATH[] = \"${DATA_PATH}\";"
  "inline constexpr double FLOAT_TOL = ${FLOAT_TOL};"
  )
file(GENERATE
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/TestData.h"
  CONTENT "inline constexpr char DATA_PATH[] = \"${DATA_PATH}\";\r\ninline constexpr double FLOAT_TOL = ${FLOAT_TOL};"
  )
set(TEST_DATA_INCLUDE ${CMAKE_CURRENT_BINARY_DIR})

# Download and unpack googletest at configure time
configure_file(cmake/DownloadGTest.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

## BUILD TESTS ##
add_subdirectory(correspondences)
add_subdirectory(transforms)
add_subdirectory(nmsac)
