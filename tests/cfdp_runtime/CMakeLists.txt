include(GoogleTest)

file(GLOB TESTS "*.cpp")

add_executable(runtime_tests ${TESTS})
target_link_libraries(runtime_tests cfdp_runtime gtest_main gmock_main)

gtest_discover_tests(runtime_tests)
