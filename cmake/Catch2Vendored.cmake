# Catch2 v3 is vendored as its single-file "amalgamated" distribution
# (third_party/catch2), so configuring needs no network access.
# It is compiled once into a static library and shared by every week.

set(SDP_CATCH2_DIR "${CMAKE_CURRENT_LIST_DIR}/../third_party/catch2")

add_library(sdp_catch2 STATIC "${SDP_CATCH2_DIR}/catch_amalgamated.cpp")
target_include_directories(sdp_catch2 SYSTEM PUBLIC "${SDP_CATCH2_DIR}")
target_compile_features(sdp_catch2 PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(sdp_catch2 PUBLIC /utf-8)
endif()

list(APPEND CMAKE_MODULE_PATH "${SDP_CATCH2_DIR}")
include(Catch)
