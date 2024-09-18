cmake_minimum_required(VERSION 3.15...3.30)

project(
    cfdp
    VERSION 0.0.1
    DESCRIPTION "CCSDS 727.0-B-5 File Delivery Protocol (CFDP)"
    LANGUAGES CXX
)

option(COMPILE_TESTS "compile tests")
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# Due to https://github.com/llvm/llvm-project/issues/108455 we cannot support this feature yet.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
set(CLANG_TIDY_COMMAND
    "${CLANG_TIDY_EXE}"
    "-config-file=${cfdp_SOURCE_DIR}/.clang-tidy"
)

add_subdirectory(src)

if(${COMPILE_TESTS})
    enable_testing()
    add_subdirectory(tests)
endif()
