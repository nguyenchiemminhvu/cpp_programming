# C++20 › Modules

## Overview
Modules replace the `#include` preprocessor model with an explicit, compiled
interface unit system. They improve build times, eliminate macro leakage, and
allow the compiler to process each module once.

## Key Concepts

| Term | Meaning |
|------|---------|
| **Module interface unit** (`.cppm` / `.ixx`) | Declares what the module exports |
| **Module implementation unit** (`.cpp`) | Contains definitions not in the interface |
| `export module MyModule;` | Declares the module name in its interface unit |
| `import MyModule;` | Imports a module into a translation unit |
| `export` (entity) | Makes a declaration visible to importers |

## Minimal Example
```cpp
// math.cppm — module interface unit
export module math;

export int add(int a, int b) { return a + b; }

// main.cpp — consumer
import math;
int main() { return add(2, 3); }
```

## Module vs Header
| | Header (`#include`) | Module (`import`) |
|-|--------------------|--------------------|
| Parsed each TU | Yes | No — compiled once |
| Macro leakage | Yes | No |
| Order sensitivity | Yes | No |
| ODR risks | High | Low |
| Incremental builds | Slow | Fast |

## Toolchain Requirements (as of 2025)
| Compiler | Min version |
|----------|-------------|
| GCC | 11+ (partial), 14 (good) |
| Clang | 16+ |
| MSVC | 2019 v16.10+ |
| CMake | 3.28+ for `CXX_MODULES` support |

## Study Checklist
- [ ] Create a named module with exported functions and classes
- [ ] Use `module :private;` to hide implementation details
- [ ] Import a standard library module (`import std;`) on a supporting toolchain
- [ ] Measure compile-time improvement of modules vs equivalent headers
- [ ] Set up CMake 3.28 `FILE_SET CXX_MODULES` for a module build

## References
- [cppreference — Modules](https://en.cppreference.com/w/cpp/language/modules)
- [MSVC Modules documentation](https://learn.microsoft.com/en-us/cpp/cpp/modules-cpp)
