cmake_minimum_required(VERSION 3.10)
project(CollocProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

set( COMMON src/fact.cpp)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(Tests tests/unit_tests.cpp ${COMMON})
target_link_libraries(Tests PRIVATE GTest::gtest_main)

add_library(Colllloc src/fact.cpp)

include(GoogleTest)
gtest_discover_tests(Tests)

