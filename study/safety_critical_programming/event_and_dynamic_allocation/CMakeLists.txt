# ── Safety-Critical › event_and_dynamic_allocation ───────────────────────────
# Lesson 1: Event-Driven System & Dynamic Callback Allocation.
# Contrasts a heap-based std::function observer against a zero-allocation,
# fixed-size inplace_function dispatcher suitable for automotive ECUs.
add_study_target(
    NAME     safety_event_and_dynamic_allocation
    STANDARD 17
    SOURCES  main.cpp
)
