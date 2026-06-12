# ── C++11 › future_promise ────────────────────────────────────────────────────
find_package(Threads REQUIRED)

add_study_target(
    NAME     cpp11_future_promise
    STANDARD 11
    SOURCES  main.cpp
)

target_link_libraries(cpp11_future_promise PRIVATE Threads::Threads)
