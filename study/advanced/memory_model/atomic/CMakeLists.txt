# ── Advanced › atomic ─────────────────────────────────────────────────────────
find_package(Threads REQUIRED)

add_study_target(
    NAME     advanced_atomic
    STANDARD 11
    SOURCES  main.cpp
)

target_link_libraries(advanced_atomic PRIVATE Threads::Threads)
