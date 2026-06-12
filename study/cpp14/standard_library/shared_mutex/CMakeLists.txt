# ── C++14 › shared_mutex ──────────────────────────────────────────────────────
find_package(Threads REQUIRED)

add_study_target(
    NAME     cpp14_shared_mutex
    STANDARD 14
    SOURCES  main.cpp
)

target_link_libraries(cpp14_shared_mutex PRIVATE Threads::Threads)
