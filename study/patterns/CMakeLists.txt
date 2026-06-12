# ── Design Patterns ────────────────────────────────────────────────────────────
# Classic C++ design patterns implemented with modern C++ idioms.

message(STATUS "[Patterns] Configuring design pattern study targets")

add_subdirectory(RAII)
add_subdirectory(pimpl)
add_subdirectory(type_erasure)
add_subdirectory(CRTP)
