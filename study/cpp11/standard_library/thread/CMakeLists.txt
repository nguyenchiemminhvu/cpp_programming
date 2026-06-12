# ── C++11 › thread ────────────────────────────────────────────────────────────
find_package(Threads REQUIRED)

add_study_target(
    NAME     cpp11_thread
    STANDARD 11
    SOURCES  main.cpp
)

target_link_libraries(cpp11_thread PRIVATE Threads::Threads)
