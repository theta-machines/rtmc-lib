# CMakeLists.txt
#
# Builds `rtmc_lib.a`
#
# Usage: `cmake -S . -B build && cmake --build build`
# Testing: `ctest --test-dir build`

# Setup project
cmake_minimum_required(VERSION 3.14)
project(rtmc_lib)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set C++ standard
# (GoogleTest requires C++14 or later)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find files
include_directories(include)
file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB TESTS "test/*.cpp")

# Define targets
add_library(${PROJECT_NAME} ${SOURCES})
add_executable(${PROJECT_NAME}_test ${TESTS})

# Configure CMake
set(CMAKE_STATIC_LIBRARY_PREFIX "") # remove "lib" prefix from target filename
cmake_policy(SET CMP0135 NEW) # download/extract timestamp policy

# Download GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Link everything for testing
target_link_libraries(
    ${PROJECT_NAME}_test
    ${PROJECT_NAME}
    GTest::gtest_main
)

# Finish configuring GoogleTest
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}_test)
