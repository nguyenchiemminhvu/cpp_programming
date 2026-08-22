# ── Safety-Critical › data_serialization_and_protocol_parsing ────────────────
# Lesson 2: Data Serialization & Protocol Parsing (SOME/IP & CAN-FD).
# Contrasts a reinterpret_cast-based parser against a zero-copy, alignment-safe,
# endian-explicit stream parser built with std::string_view and bit shifting.
add_study_target(
    NAME     safety_data_serialization_and_protocol_parsing
    STANDARD 17
    SOURCES  main.cpp
)
