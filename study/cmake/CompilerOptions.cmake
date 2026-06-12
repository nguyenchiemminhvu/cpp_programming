# ── CompilerOptions.cmake ───────────────────────────────────────────────────────
# Shared compiler settings and reusable helper functions for all study targets.
# Included once from the root CMakeLists.txt and available in every subdirectory.

# ─────────────────────────────────────────────────────────────────────────────────
# target_enable_warnings(<target>)
#
# Applies a strict, portable set of warning flags to <target>.
# ─────────────────────────────────────────────────────────────────────────────────
function(target_enable_warnings target)
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
            -Wall -Wextra -Wpedantic -Wshadow -Wno-unused-parameter>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    )
endfunction()

# ─────────────────────────────────────────────────────────────────────────────────
# add_study_target(NAME <name> STANDARD <std> SOURCES <file> [<file>…])
#
# Creates a self-contained study executable with:
#   • the requested C++ standard enforced
#   • extensions disabled
#   • strict warnings enabled
#
# Usage example (inside a feature CMakeLists.txt):
#
#   add_study_target(
#       NAME     cpp11_lambda
#       STANDARD 11
#       SOURCES  main.cpp
#   )
# ─────────────────────────────────────────────────────────────────────────────────
function(add_study_target)
    cmake_parse_arguments(ARG "" "NAME;STANDARD" "SOURCES" ${ARGN})

    if(NOT ARG_NAME)
        message(FATAL_ERROR "add_study_target: NAME is required")
    endif()
    if(NOT ARG_STANDARD)
        message(FATAL_ERROR "add_study_target: STANDARD is required")
    endif()
    if(NOT ARG_SOURCES)
        message(FATAL_ERROR "add_study_target: SOURCES is required")
    endif()

    add_executable(${ARG_NAME} ${ARG_SOURCES})

    set_target_properties(${ARG_NAME} PROPERTIES
        CXX_STANDARD          ${ARG_STANDARD}
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS        OFF
    )

    target_enable_warnings(${ARG_NAME})
endfunction()
