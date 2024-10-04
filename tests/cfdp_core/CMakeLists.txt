include(GoogleTest)

file(GLOB TESTS "*.cpp")

add_executable(core_tests ${TESTS})
target_link_libraries(core_tests cfdp_core gtest_main gmock_main)

gtest_discover_tests(core_tests)
