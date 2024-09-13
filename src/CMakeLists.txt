set(HEADER_LIST "${cfdp_SOURCE_DIR}/include/cfdp/cfdp.hpp")

add_library(cfdp_lib SHARED cfdp.cpp ${HEADER_LIST})

target_include_directories(cfdp_lib PUBLIC ../include)
target_compile_features(cfdp_lib PUBLIC cxx_std_20)
