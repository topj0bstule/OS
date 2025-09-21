enable_testing()
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(test_thread test_thread.cpp)
target_link_libraries(test_thread GTest::gtest_main thread_code)

include(GoogleTest)
gtest_discover_tests(test_thread)
