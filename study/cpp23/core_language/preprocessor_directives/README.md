# C++23 › Preprocessor Directives (`#warning`, `#elifdef`, `#elifndef`)

## Overview
C++23 standardises three preprocessor tools that have existed as compiler
extensions for years:

- **P2437** — `#warning "message"` — like `#error`, but non-fatal.
- **P2334** — `#elifdef X` — shorthand for `#elif defined(X)`.
- **P2334** — `#elifndef X` — shorthand for `#elif !defined(X)`.

They align C++ with C23 and standardise what GCC, Clang, and MSVC already
implement.

## `#warning`
Emits a compiler diagnostic (typically a warning) with the given message and
**continues compilation**.
```cpp
#if defined(NDEBUG)
#warning "Building release with assertions disabled"
#endif
```
Contrast with `#error`, which stops the compilation.

## `#elifdef` / `#elifndef`
Cleaner conditional compilation chains:
```cpp
#if   defined(_WIN32)
    #include "windows_impl.h"
#elifdef __linux__
    #include "linux_impl.h"
#elifdef __APPLE__
    #include "mac_impl.h"
#elifndef PLATFORM_KNOWN
    #error "Unsupported platform"
#endif
```

### Equivalents
| C++23 | Equivalent to |
|---|---|
| `#elifdef X` | `#elif defined(X)` |
| `#elifndef X` | `#elif !defined(X)` |

They match the existing `#ifdef` / `#ifndef` shorthand pattern.

## When to Use `#warning`
- Flagging non-portable code paths on a specific compiler / OS.
- Warning about deprecated build configurations.
- Noting temporary shims or TODOs that must survive to the build log.
- Signalling missing feature-test macros without breaking the build.

## Availability
GCC and Clang have supported `#warning` since forever; MSVC added it later.
All three vendors now conform to the standardised form.

## Study Checklist
- [ ] Replace a chain of `#elif defined(X)` with `#elifdef X`
- [ ] Add `#warning` for a deprecated build option and confirm the message appears in build output
- [ ] Confirm `#warning` does **not** stop compilation
- [ ] Test `#elifndef` for a "fallback" branch when no known platform macro is set
- [ ] Compare diagnostic output between GCC, Clang, and MSVC

## References
- [P2437 — Support for `#warning`](https://wg21.link/P2437)
- [P2334 — `#elifdef` and `#elifndef`](https://wg21.link/P2334)
- [cppreference — conditional inclusion](https://en.cppreference.com/w/cpp/preprocessor/conditional)
