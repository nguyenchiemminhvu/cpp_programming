# ── C++20 Study Module ─────────────────────────────────────────────────────────
# C++20 (ISO/IEC 14882:2020) — the largest revision since C++11.
# Major themes: concepts, ranges, coroutines, modules, std::format, three-way
#               comparison (spaceship operator), consteval, std::span.

message(STATUS "[C++20] Configuring study targets")

add_subdirectory(core_language)
add_subdirectory(standard_library)
