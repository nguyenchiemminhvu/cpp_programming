# C++14 — Study Overview
> ISO/IEC 14882:2014 · Incremental refinement of C++11

## Why C++14 Matters
C++14 is a "bug-fix + polish" release: it relaxes constraints introduced in C++11,
fills small gaps (e.g., the missing `make_unique`), and improves ergonomics.
It is a prerequisite for C++17 concepts and sets the foundation for `decltype(auto)`.

---

## Study Map

### `core_language/` — Language Refinements
| Folder | Feature | Key Idea |
|--------|---------|---------|
| `generic_lambda/` | `auto` parameters in lambdas | Lambdas with templated `operator()` |
| `return_type_deduction/` | `auto` return for functions | Deduce return type from body, not just lambdas |
| `relaxed_constexpr/` | Relaxed `constexpr` | Multi-statement, loop-capable `constexpr` functions |

### `standard_library/` — Library Additions
| Folder | Feature | Key Idea |
|--------|---------|---------|
| `make_unique/` | `std::make_unique` | Exception-safe factory for `unique_ptr` |
| `shared_mutex/` | `std::shared_mutex` | Readers-writer lock for concurrent access |

---

## Additional C++14 Topics (self-study)
- **`decltype(auto)`** return deduction — preserves references unlike plain `auto`
- **Binary literals** — `0b1010'0011` (also digit separators `'`)
- **Variable templates** — `template<typename T> constexpr T pi = T(3.14159…)`
- **Deprecated `[[deprecated]]` attribute** — mark APIs for removal
- **`std::integer_sequence`** — compile-time index sequences for tuple manipulation
- **`std::exchange`** — atomic set-and-return-old-value utility
- **`std::quoted`** — I/O manipulator for quoted strings
- **Heterogeneous lookup in ordered containers** — `map::find` accepting non-key type

## Recommended Resources
- *Effective Modern C++* — Scott Meyers
- [cppreference C++14](https://en.cppreference.com/w/cpp/14)
