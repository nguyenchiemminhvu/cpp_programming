# ── C++20 › stop_token ────────────────────────────────────────────────────────
find_package(Threads REQUIRED)

add_study_target(
    NAME     cpp20_stop_token
    STANDARD 20
    SOURCES  main.cpp
)

target_link_libraries(cpp20_stop_token PRIVATE Threads::Threads)
