cmake_minimum_required(VERSION 3.11...3.20)
project(CppOptimization)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
set(BENCHMARK_ENABLE_TESTING OFF)

find_package(Threads)

include(FetchContent)

FetchContent_Declare(
    google_benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG main
)

FetchContent_MakeAvailable(google_benchmark)

function(compile_executables target_name file_path)
    add_executable(${target_name} ${file_path})
    target_link_libraries(${target_name}
        PRIVATE
            benchmark::benchmark
            Threads::Threads
    )
endfunction()

compile_executables(benchmark_setup_test ${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark_setup_test.cpp)
compile_executables(case_study_strings ${CMAKE_CURRENT_SOURCE_DIR}/src/case_study_strings.cpp)
compile_executables(case_study_hot_statements ${CMAKE_CURRENT_SOURCE_DIR}/src/case_study_hot_statements.cpp)
compile_executables(case_study_search_sort ${CMAKE_CURRENT_SOURCE_DIR}/src/case_study_search_sort.cpp)
compile_executables(case_study_input_output ${CMAKE_CURRENT_SOURCE_DIR}/src/case_study_input_output.cpp)
compile_executables(case_study_memory_management ${CMAKE_CURRENT_SOURCE_DIR}/src/case_study_memory_management.cpp)
compile_executables(case_study_virtual_and_template ${CMAKE_CURRENT_SOURCE_DIR}/src/case_study_virtual_and_template.cpp)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/files)
add_custom_target(copy_files ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/files/* ${CMAKE_CURRENT_BINARY_DIR}/files
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/files/sample_file.txt
)