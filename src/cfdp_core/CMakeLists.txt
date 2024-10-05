file(
    GLOB
    HEADER_LIST
    CONFIGURE_DEPENDS
    "${cfdp_SOURCE_DIR}/include/core/cfdp_core/*.hpp"
)
file(GLOB SOURCE_LIST "*.cpp")

add_library(cfdp_core ${SOURCE_LIST} ${HEADER_LIST})

target_include_directories(
    cfdp_core
    PUBLIC
    "${cfdp_SOURCE_DIR}/include/core"
)
target_compile_features(cfdp_core PUBLIC cxx_std_23)

set_target_properties(cfdp_core PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
