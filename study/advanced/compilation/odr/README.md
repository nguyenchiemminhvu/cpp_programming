# Advanced › One Definition Rule (ODR)

## Overview
The **One Definition Rule** states that:
1. Every TU that *uses* an entity must contain exactly one *definition* — unless it has only a declaration.
2. Across all TUs, each **non-inline** entity has exactly one definition.
3. **Inline** entities may be defined in multiple TUs, but all definitions must be identical.

## ODR Violation Examples

### Non-inline function defined in two TUs
```cpp
// file_a.cpp
int f() { return 1; }

// file_b.cpp
int f() { return 2; }   // ODR violation — undefined behaviour at link time
```

### Different class layouts in two TUs
```cpp
// header.h included in two TUs with different preprocessor states
#ifdef LARGE
struct Foo { int a, b, c; };
#else
struct Foo { int a; };   // different layout — silent ODR violation
#endif
```

## Safe Patterns
- Non-inline free functions: define in exactly one `.cpp`, declare in header.
- `inline` functions: define in header (all TUs get the same definition).
- `inline constexpr` variables (C++17): define in header.
- Templates: generally header-only (implicit instantiation in each TU is fine).

## Study Checklist
- [ ] Create an intentional ODR violation; observe the linker behaviour
- [ ] Use `-Wodr` (Clang) or LTO+`-Wodr` to detect ODR violations
- [ ] Explain why violating ODR on class types (different members) is especially dangerous
- [ ] Show that `inline constexpr` in C++17 headers is ODR-safe

## References
- [cppreference — Definitions and ODR](https://en.cppreference.com/w/cpp/language/definition)
