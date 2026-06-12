# C++11 › Perfect Forwarding Deep Dive

## Overview
Perfect forwarding preserves the value category (lvalue/rvalue) and cv-qualifiers
of arguments when passing them through a template function chain. It requires
**forwarding references** and `std::forward`.

## Forwarding Reference (`T&&` in a deduced context)
```cpp
template<typename T>
void wrapper(T&& arg) {              // T&& here is a FORWARDING reference
    target(std::forward<T>(arg));    // preserves lvalue/rvalue nature
}
```
**Critical**: `T&&` is a forwarding reference **only** when `T` is a deduced
template parameter. `Widget&&` is always an rvalue reference.

## Reference Collapsing (the mechanism)
| Declared | `T` deduced as | Result |
|----------|----------------|--------|
| `T&&` | `int&` (lvalue passed) | `int&` |
| `T&&` | `int` (rvalue passed) | `int&&` |

Rule: only `&&` + `&&` → `&&`; everything else collapses to `&`.

## Common Patterns
```cpp
// Factory function
template<typename T, typename... Args>
std::unique_ptr<T> make(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// Emplace-style insertion
template<typename... Args>
void emplace(Args&&... args) {
    container_.emplace_back(std::forward<Args>(args)...);
}
```

## Pitfalls
- Never `std::forward` the same argument twice.
- `std::move` and `std::forward` are **casts**, not operations.
- Passing `const T` through `T&&` deduces `const T&`, not `T&&`.

## Study Checklist
- [ ] Trace reference collapsing for `wrapper(lvalue)` and `wrapper(rvalue)`
- [ ] Show that double-forwarding causes use-after-move
- [ ] Implement variadic `make_unique` using perfect forwarding
- [ ] Demonstrate that forwarding a `const` lvalue gives `const T&`, not `T`

## References
- [cppreference — std::forward](https://en.cppreference.com/w/cpp/utility/forward)
- *Effective Modern C++*, Scott Meyers — Items 23–28
