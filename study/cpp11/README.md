# C++11 — Study Overview
> ISO/IEC 14882:2011 · The foundational modern C++ standard

## Why C++11 Matters
C++11 was the most transformative revision in the language's history. It introduced
move semantics, lambdas, smart pointers, and a portable threading model — concepts
that define idiomatic C++ today. Mastering C++11 is prerequisite to every later standard.

---

## Study Map

### `core_language/` — Language Feature Changes
| Folder | Feature | Key Idea |
|--------|---------|---------|
| `auto/` | Type deduction | Let the compiler deduce types from initializers |
| `decltype/` | Declared type | Query the exact type of an expression |
| `lambda/` | Lambda expressions | Inline anonymous callable objects (closures) |
| `rvalue_references/` | Move semantics | Transfer resources instead of copying |
| `constexpr/` | Compile-time eval | Compute constants and simple functions at compile time |
| `static_assert/` | Compile-time checks | Assert invariants without runtime cost |

### `standard_library/` — Library Additions
| Folder | Feature | Key Idea |
|--------|---------|---------|
| `smart_pointers/` | `unique_ptr`, `shared_ptr`, `weak_ptr` | RAII-based automatic memory management |
| `thread/` | `<thread>`, `<mutex>`, `<future>` | Portable concurrency primitives |
| `chrono/` | `<chrono>` | Type-safe clocks, durations, and time points |
| `tuple/` | `std::tuple` | Heterogeneous fixed-size value collection |
| `unordered_containers/` | `unordered_map/set` | Hash-based O(1) average lookup containers |

### `utilities/` — Low-Level Utilities
| Folder | Feature | Key Idea |
|--------|---------|---------|
| `move_forward/` | `std::move`, `std::forward` | Cast utilities for move semantics and perfect forwarding |
| `type_traits/` | `<type_traits>` | Compile-time type introspection and transformation |

---

## Additional C++11 Topics (self-study)
- **Range-based `for`** — `for (auto& x : container)`
- **Initializer lists** — `std::initializer_list<T>`
- **Variadic templates** — `template<typename... Ts>`
- **Delegating constructors** — call another constructor of the same class
- **`nullptr`** — type-safe null pointer constant replacing `NULL`
- **Scoped enumerations** — `enum class Color { Red, Green, Blue };`
- **`override` / `final`** — explicit virtual override markers
- **Default / deleted functions** — `= default`, `= delete`
- **User-defined literals** — `42_km`, `3.14_rad`

## Recommended Resources
- *Effective Modern C++* — Scott Meyers (Items 1–42)
- [cppreference C++11](https://en.cppreference.com/w/cpp/11)
- *The C++ Programming Language*, 4th ed — Bjarne Stroustrup
