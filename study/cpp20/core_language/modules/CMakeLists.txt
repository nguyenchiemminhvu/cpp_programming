# ── C++20 › modules ───────────────────────────────────────────────────────────
# Note: full module support (.cppm / import std;) requires CMake ≥ 3.28.
# This entry point uses a traditional .cpp file.
# Upgrade to FILE_SET CXX_MODULES when your toolchain is ready.
add_study_target(
    NAME     cpp20_modules
    STANDARD 20
    SOURCES  main.cpp
)
