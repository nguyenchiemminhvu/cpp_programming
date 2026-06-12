# C++20 — Study Overview
> ISO/IEC 14882:2020 · The largest C++ revision since C++11

## Why C++20 Matters
C++20 delivers four headline features — **Concepts**, **Ranges**, **Coroutines**,
and **Modules** — that fundamentally change how generic, functional, and asynchronous
C++ code is written. Combined with `std::format`, `std::span`, and the extended
`<chrono>` calendar, C++20 closes long-standing gaps vs Python, Rust, and Java.

---

## Study Map

### `core_language/` — Language Feature Changes
| Folder | Feature | Key Idea |
|--------|---------|---------|
| `concepts/` | Concepts & constraints | Named, reusable template parameter requirements |
| `ranges/` | Ranges & views | Composable, lazy, pipeline-based sequence processing |
| `coroutines/` | Coroutines | Suspendable/resumable functions for async and generators |
| `modules/` | Modules | Compiled interface units replacing `#include` |

### `standard_library/` — Library Additions
| Folder | Feature | Key Idea |
|--------|---------|---------|
| `span/` | `std::span<T>` | Non-owning contiguous range view |
| `format/` | `std::format` | Type-safe Python-style string formatting |
| `calendar/` | Calendar & time zones | Full `<chrono>` calendar and IANA time zone support |

---

## Additional C++20 Topics (self-study)
- **Three-way comparison (spaceship operator `<=>`)** — auto-generate all relational operators
- **`consteval`** — functions that *must* evaluate at compile time
- **`constinit`** — guarantee constant initialisation of static variables
- **`std::jthread`** — joining thread with cooperative cancellation (`stop_token`)
- **Designated initialisers** — `Point p{.x = 1, .y = 2};`
- **`[[likely]]` / `[[unlikely]]`** attributes — branch prediction hints
- **Lambda improvements** — `[=, this]` capture, template lambdas `[]<typename T>(T x){}`
- **`std::source_location`** — compile-time file/line info (replaces `__FILE__`/`__LINE__`)
- **`std::bit_cast`** — safe reinterpretation of object representations
- **Abbreviated function templates** — `void f(auto x)` = `template<typename T> void f(T x)`

## Toolchain Support Notes
| Feature | GCC | Clang | MSVC |
|---------|-----|-------|------|
| Concepts | 10+ | 10+ | 2019 |
| Ranges | 10+ | 13+ | 2019 v16.6 |
| Coroutines | 11+ | 12+ | 2019 v16.8 |
| Modules | 11+ (partial) | 16+ | 2019 v16.10 |
| `std::format` | 13+ | 14+ (libc++) | 2019 v16.10 |

## Recommended Resources
- [cppreference C++20](https://en.cppreference.com/w/cpp/20)
- *C++20: The Complete Guide* — Nicolai M. Josuttis
- *C++ Coroutines* — [lewissbaker/cppcoro](https://github.com/lewissbaker/cppcoro)
