cmake_minimum_required(VERSION 3.14)
project(l1_ue_simulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# MISRA / High Reliability Warning Flags
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

find_package(nlohmann_json REQUIRED)

add_library(${PROJECT_NAME}_lib STATIC
    src/telemetry_server.cpp
)
target_include_directories(${PROJECT_NAME}_lib PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(${PROJECT_NAME}_lib PUBLIC nlohmann_json::nlohmann_json pthread)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)

# Tests
enable_testing()
add_executable(${PROJECT_NAME}_test tests/tests.cpp)
target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME}_lib)
add_test(NAME CoreTest COMMAND ${PROJECT_NAME}_test)
