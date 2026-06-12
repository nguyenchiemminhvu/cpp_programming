# ── C++17 › scoped_lock ───────────────────────────────────────────────────────
find_package(Threads REQUIRED)

add_study_target(
    NAME     cpp17_scoped_lock
    STANDARD 17
    SOURCES  main.cpp
)

target_link_libraries(cpp17_scoped_lock PRIVATE Threads::Threads)
