cmake_minimum_required(VERSION 3.15...3.30)

project(cfdp
        VERSION 0.0.1
        DESCRIPTION "CCSDS 727.0-B-5 File Delivery Protocol (CFDP)"
        LANGUAGES CXX)

set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)
