cmake_minimum_required(VERSION 3.21)
project(sdp_seminar_2026_2027 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Optional: a private checkout that holds reference solutions
# (weeks/NN-*/solutions/). The instructor repo sets this before
# add_subdirectory()-ing this project; students never need it.
set(SDP_PRIVATE_ROOT "" CACHE PATH "Root of a checkout with private solutions")

enable_testing()
include(cmake/Catch2Vendored.cmake)
include(cmake/SdpWeek.cmake)

file(GLOB SDP_WEEK_DIRS LIST_DIRECTORIES true
     "${CMAKE_CURRENT_SOURCE_DIR}/weeks/[0-9][0-9]-*")
list(SORT SDP_WEEK_DIRS)
foreach(week_dir IN LISTS SDP_WEEK_DIRS)
    if(IS_DIRECTORY "${week_dir}/tests")
        sdp_add_week("${week_dir}")
    endif()
endforeach()
