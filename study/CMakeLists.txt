cmake_minimum_required(VERSION 3.20)

project(ModernCppStudy
    VERSION 1.0.0
    DESCRIPTION "Structured Modern C++ Standards Learning Repository"
    LANGUAGES CXX
)

# ── Global Defaults ─────────────────────────────────────────────────────────────
set(CMAKE_CXX_EXTENSIONS        OFF)   # Disable GNU/MSVC extensions; maximise portability
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # Enables clangd / IDE tooling

# ── Shared CMake Modules ────────────────────────────────────────────────────────
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerOptions)

# ── Per-Standard Subdirectories ─────────────────────────────────────────────────
# Each standard is an independent sub-tree; disable any you have not yet reached.
add_subdirectory(cpp11)
add_subdirectory(cpp14)
add_subdirectory(cpp17)
add_subdirectory(cpp20)
add_subdirectory(cpp23)

# ── Cross-Standard Advanced Topics ─────────────────────────────────────────────
add_subdirectory(advanced)
add_subdirectory(patterns)

# ── Safety-Critical Programming ───────────────────────────────────────────────
add_subdirectory(safety_critical_programming)
