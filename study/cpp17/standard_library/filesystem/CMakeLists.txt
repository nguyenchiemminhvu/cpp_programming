# ── C++17 › filesystem ────────────────────────────────────────────────────────
add_study_target(
    NAME     cpp17_filesystem
    STANDARD 17
    SOURCES  main.cpp
)

# GCC < 9 workaround — link stdc++fs for older toolchains
target_link_libraries(cpp17_filesystem PRIVATE
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>
)
