file(
    GLOB
    HEADER_LIST
    CONFIGURE_DEPENDS
    "${cfdp_SOURCE_DIR}/include/runtime/cfdp_runtime/*.hpp"
)
file(GLOB SOURCE_LIST "*.cpp")

add_library(cfdp_runtime ${SOURCE_LIST} ${HEADER_LIST})

target_include_directories(
    cfdp_runtime
    PUBLIC
    "${cfdp_SOURCE_DIR}/include/runtime"
)
target_compile_features(cfdp_runtime PUBLIC cxx_std_23)
target_link_libraries(cfdp_runtime PRIVATE cfdp_core)

set_target_properties(cfdp_runtime PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
