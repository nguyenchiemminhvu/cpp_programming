# C++20 › Designated Initializers

## Overview
Designated initializers allow aggregate struct/class members to be initialised
by name, making initialization self-documenting and resilient to member reordering.

## Syntax
```cpp
struct Config {
    int    width      = 800;
    int    height     = 600;
    bool   fullscreen = false;
    float  scale      = 1.0f;
};

Config cfg{.width = 1920, .height = 1080, .fullscreen = true};
// .scale omitted — uses default value 1.0f
```

## Rules
- Designators must appear in **declaration order** (unlike C99).
- Works on **aggregates** only (no user-provided constructors, no `private` / `protected` members).
- Omitted members receive their **default member initialiser**, or are value-initialised.
- Cannot mix designated and non-designated initialisers in the same `{…}`.

## Study Checklist
- [ ] Initialise a config struct with designated initialisers; skip optional fields
- [ ] Show the compile error for out-of-order designators
- [ ] Compare readability: positional init vs designated init for a 6-member struct
- [ ] Use as inline function argument: `create_window({.width=1280, .height=720})`

## References
- [cppreference — Aggregate initialization (designated)](https://en.cppreference.com/w/cpp/language/aggregate_initialization#Designated_initializers)
