include(FetchContent)
include(GoogleTest)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(googletest)

file(GLOB TESTS "*.cpp")

add_executable(tests ${TESTS})
target_link_libraries(tests cfdp_lib gtest_main gmock_main)

gtest_discover_tests(tests)
