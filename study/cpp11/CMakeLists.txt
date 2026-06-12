# ── C++11 Study Module ─────────────────────────────────────────────────────────
# C++11 (ISO/IEC 14882:2011) — the foundational modern C++ standard.
# Major themes: move semantics, lambdas, type inference, concurrency, smart pointers.

message(STATUS "[C++11] Configuring study targets")

add_subdirectory(core_language)
add_subdirectory(standard_library)
add_subdirectory(utilities)
