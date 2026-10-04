# sdp_add_week(<week_dir>)
#
# Every week folder has the same shape:
#   starter/    code students complete (headers and/or .cpp files)
#   tests/      Catch2 tests; they FAIL until starter/ is completed
#   bench/      optional timing programs (main() of their own)
#   solutions/  reference code with the same file names as starter/
#               (empty in the public repo until released)
#
# For week "NN-slug" this creates:
#   wNN_tests            tests built against starter/   (ctest label: starter)
#   wNN_bench            bench built against starter/   (if bench/ exists)
#   wNN_solution_tests   tests built against solutions/ (ctest label: solution)
#   wNN_solution_bench   bench built against solutions/
# The solution targets exist only when solution sources are available,
# either in the week's own solutions/ or under SDP_PRIVATE_ROOT.

function(_sdp_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive- /utf-8)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
    endif()
endfunction()

function(_sdp_add_variant num variant code_dir week_dir)
    file(GLOB code_src CONFIGURE_DEPENDS "${code_dir}/*.cpp")
    file(GLOB test_src CONFIGURE_DEPENDS "${week_dir}/tests/*.cpp")
    file(GLOB bench_src CONFIGURE_DEPENDS "${week_dir}/bench/*.cpp")

    if(variant STREQUAL "starter")
        set(prefix "w${num}")
    else()
        set(prefix "w${num}_${variant}")
    endif()

    add_executable(${prefix}_tests ${test_src} ${code_src})
    target_include_directories(${prefix}_tests PRIVATE "${code_dir}")
    target_link_libraries(${prefix}_tests PRIVATE sdp_catch2)
    _sdp_warnings(${prefix}_tests)
    catch_discover_tests(${prefix}_tests
        TEST_PREFIX "w${num}/${variant}: "
        DISCOVERY_MODE PRE_TEST
        PROPERTIES LABELS "${variant}")

    if(bench_src)
        add_executable(${prefix}_bench ${bench_src} ${code_src})
        target_include_directories(${prefix}_bench PRIVATE "${code_dir}")
        _sdp_warnings(${prefix}_bench)
    endif()
endfunction()

function(sdp_add_week week_dir)
    get_filename_component(week_name "${week_dir}" NAME)
    string(SUBSTRING "${week_name}" 0 2 num)

    file(GLOB any_tests CONFIGURE_DEPENDS "${week_dir}/tests/*.cpp")
    if(NOT any_tests)
        return()  # a week that has no tests (yet)
    endif()

    _sdp_add_variant(${num} starter "${week_dir}/starter" "${week_dir}")

    file(GLOB public_solutions "${week_dir}/solutions/*.h" "${week_dir}/solutions/*.cpp")
    set(solutions_dir "")
    if(public_solutions)
        set(solutions_dir "${week_dir}/solutions")
    elseif(SDP_PRIVATE_ROOT AND IS_DIRECTORY "${SDP_PRIVATE_ROOT}/weeks/${week_name}/solutions")
        set(solutions_dir "${SDP_PRIVATE_ROOT}/weeks/${week_name}/solutions")
    endif()

    if(solutions_dir)
        _sdp_add_variant(${num} solution "${solutions_dir}" "${week_dir}")
    endif()
endfunction()
