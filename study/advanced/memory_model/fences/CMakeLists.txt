# ── Advanced › fences ─────────────────────────────────────────────────────────
find_package(Threads REQUIRED)

add_study_target(
    NAME     advanced_fences
    STANDARD 11
    SOURCES  main.cpp
)

target_link_libraries(advanced_fences PRIVATE Threads::Threads)
