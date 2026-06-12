# ── Advanced › memory_order ───────────────────────────────────────────────────
find_package(Threads REQUIRED)

add_study_target(
    NAME     advanced_memory_order
    STANDARD 11
    SOURCES  main.cpp
)

target_link_libraries(advanced_memory_order PRIVATE Threads::Threads)
