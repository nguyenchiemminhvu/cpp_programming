# ── C++11 › mutex_locks ───────────────────────────────────────────────────────
find_package(Threads REQUIRED)

add_study_target(
    NAME     cpp11_mutex_locks
    STANDARD 11
    SOURCES  main.cpp
)

target_link_libraries(cpp11_mutex_locks PRIVATE Threads::Threads)
