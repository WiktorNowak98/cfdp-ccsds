file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${cfdp_SOURCE_DIR}/include/cfdp/*.hpp")
add_library(cfdp_lib SHARED cfdp_lib.cpp ${HEADER_LIST})

target_include_directories(cfdp_lib PUBLIC "${cfdp_SOURCE_DIR}/include/")
target_compile_features(cfdp_lib PUBLIC cxx_std_20)
set_target_properties(cfdp_lib PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
