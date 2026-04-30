cmake_minimum_required(VERSION 3.16)
project(host_tests)

# GoogleTest requires at least C++14, we are using C++17
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  # Using a recent known-good commit/release, or standard archive link
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Define our test executable
add_executable(test_ringbuffer 
    test_ringbuffer.cpp 
    test_semaphore.cpp
)

# Include directories
target_include_directories(test_ringbuffer PRIVATE ../components/core_logic/include)

# Link against GoogleTest's main library (provides main() automatically)
target_link_libraries(test_ringbuffer
  GTest::gtest_main
)

# Discover and run tests via CTest
include(GoogleTest)
gtest_discover_tests(test_ringbuffer)
