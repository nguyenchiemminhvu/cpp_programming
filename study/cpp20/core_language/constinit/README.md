# C++20 › `constinit` — Constant Initialisation Guarantee

## Overview
`constinit` asserts that a variable with static or thread-local storage
duration is **constant-initialized** at compile time. It does **not** make
the variable immutable — it only guarantees the initialisation timing.

## Motivation: Static Initialisation Order Fiasco
```cpp
// file_a.cpp
int x = compute();          // dynamic init — order undefined across TUs

// file_b.cpp (may run before file_a.cpp)
extern int x;
int y = x * 2;              // UB if x hasn't been initialised yet
```

## `constinit` Solution
```cpp
constinit int counter = 0;  // guaranteed compile-time zero-initialisation
                              // still mutable at runtime
++counter;                   // OK — constinit does not imply const
```

## `constexpr` vs `constinit` vs `const`
| | Compile-time init | Mutable at runtime | Implies `const` |
|-|-------------------|--------------------|-----------------|
| `constexpr` | Yes (enforced) | No | Yes (for variables) |
| `constinit` | Yes (enforced) | Yes | No |
| `const` | No (just immutable) | No | Yes |

## Study Checklist
- [ ] Mark a global counter `constinit`; confirm it compiles with a constant initialiser
- [ ] Show that `constinit` errors when the initialiser is not a constant expression
- [ ] Demonstrate the static initialisation order fiasco; fix it with `constinit`
- [ ] Contrast with `constexpr` for a global that must be modified at runtime

## References
- [cppreference — constinit](https://en.cppreference.com/w/cpp/language/constinit)
