# ── C++20 › jthread ───────────────────────────────────────────────────────────
find_package(Threads REQUIRED)

add_study_target(
    NAME     cpp20_jthread
    STANDARD 20
    SOURCES  main.cpp
)

target_link_libraries(cpp20_jthread PRIVATE Threads::Threads)
