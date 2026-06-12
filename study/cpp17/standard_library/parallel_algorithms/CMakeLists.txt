# ── C++17 › parallel_algorithms ───────────────────────────────────────────────
# Note: on macOS, parallel policies may require linking against libtbb.
# On Linux with GCC, install libtbb-dev and add:
#   target_link_libraries(cpp17_parallel_algorithms PRIVATE tbb)
add_study_target(
    NAME     cpp17_parallel_algorithms
    STANDARD 17
    SOURCES  main.cpp
)
