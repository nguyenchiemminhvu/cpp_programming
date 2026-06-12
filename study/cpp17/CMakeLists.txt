# ── C++17 Study Module ─────────────────────────────────────────────────────────
# C++17 (ISO/IEC 14882:2017) — substantial additions to language and library.
# Major themes: structured bindings, if constexpr, std::optional/variant/any,
#               std::filesystem, fold expressions, deduction guides.

message(STATUS "[C++17] Configuring study targets")

add_subdirectory(core_language)
add_subdirectory(standard_library)
