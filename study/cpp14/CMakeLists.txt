# ── C++14 Study Module ─────────────────────────────────────────────────────────
# C++14 (ISO/IEC 14882:2014) — incremental refinement of C++11.
# Major themes: generic lambdas, relaxed constexpr, make_unique, shared_mutex.

message(STATUS "[C++14] Configuring study targets")

add_subdirectory(core_language)
add_subdirectory(standard_library)
