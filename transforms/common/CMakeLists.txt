set(target common)

string(CONCAT dummy_target ${target} "_transforms")

add_library(${dummy_target} INTERFACE)

# Create namespaced alias
add_library(${PROJECT_NAME}::${target} ALIAS ${dummy_target})

target_include_directories(${dummy_target}
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    #$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)
