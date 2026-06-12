# ── C++11 › condition_variable ────────────────────────────────────────────────
find_package(Threads REQUIRED)

add_study_target(
    NAME     cpp11_condition_variable
    STANDARD 11
    SOURCES  main.cpp
)

target_link_libraries(cpp11_condition_variable PRIVATE Threads::Threads)
