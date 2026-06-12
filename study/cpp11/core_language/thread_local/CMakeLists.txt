# ── C++11 › thread_local ──────────────────────────────────────────────────────
find_package(Threads REQUIRED)

add_study_target(
    NAME     cpp11_thread_local
    STANDARD 11
    SOURCES  main.cpp
)

target_link_libraries(cpp11_thread_local PRIVATE Threads::Threads)
