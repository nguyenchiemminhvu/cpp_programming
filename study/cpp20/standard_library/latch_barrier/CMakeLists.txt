# ── C++20 › latch_barrier ─────────────────────────────────────────────────────
find_package(Threads REQUIRED)

add_study_target(
    NAME     cpp20_latch_barrier
    STANDARD 20
    SOURCES  main.cpp
)

target_link_libraries(cpp20_latch_barrier PRIVATE Threads::Threads)
