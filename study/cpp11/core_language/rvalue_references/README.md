# C++11 › Rvalue References & Move Semantics

## Overview
Rvalue references (`T&&`) enable **move semantics** and **perfect forwarding**,
allowing resources to be transferred instead of copied, eliminating unnecessary
allocations in performance-critical code.

## Core Vocabulary

| Term | Meaning |
|------|---------|
| **lvalue** | Has an identity (address); persists beyond the expression |
| **rvalue** | Temporary; no persistent address |
| `T&&` | Rvalue reference — binds to rvalues and temporaries |
| `std::move(x)` | Cast to rvalue reference — enables move; does NOT move by itself |
| **Move constructor** | `T(T&&)` — transfers ownership of resources |
| **Move assignment** | `T& operator=(T&&)` — transfers ownership on assignment |

## The Rule of Five (C++11 extension of Rule of Three)
If you define any of the following, define all five:
1. Destructor
2. Copy constructor
3. Copy assignment operator
4. **Move constructor**
5. **Move assignment operator**

## Perfect Forwarding
```cpp
template<typename T>
void wrapper(T&& arg) {
    target(std::forward<T>(arg));  // preserves value category
}
```
`T&&` here is a **forwarding reference** (aka universal reference), not a plain rvalue reference.

## Study Checklist
- [ ] Implement a move-only resource class (e.g., a simple RAII file wrapper)
- [ ] Show the difference in `std::vector` performance: push_back with copy vs move
- [ ] Demonstrate `std::move` on a `std::string` and inspect the moved-from state
- [ ] Write a `make_pair`-style perfect-forwarding factory function
- [ ] Explain why `std::move` on a `const` object silently falls back to copy

## References
- [cppreference — Move constructors](https://en.cppreference.com/w/cpp/language/move_constructor)
- *Effective Modern C++*, Scott Meyers — Items 23, 24, 25, 26, 28, 29
