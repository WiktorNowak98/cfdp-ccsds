include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(googletest)

file(GLOB TESTS "*.cpp")

add_executable(tests ${TESTS})
target_link_libraries(tests GTest::gtest_main cfdp_lib)

include(GoogleTest)
gtest_discover_tests(tests)
