# C++11 › `noexcept` — Exception Specification

## Overview
`noexcept` marks a function as guaranteeing it will not propagate exceptions.
It enables compiler optimisations (especially in move operations) and can be
queried at compile time via the `noexcept` operator.

## Two Roles

### Specifier
```cpp
void f() noexcept;                  // unconditionally noexcept
void g() noexcept(condition);       // conditionally noexcept
```

### Operator
```cpp
static_assert(noexcept(f()));       // compile-time query: true if f() is noexcept
```

## Why It Matters for Move Semantics
`std::vector` only uses a move constructor (instead of copy) during reallocation
if the move constructor is `noexcept`. Mark move and swap accordingly:
```cpp
MyType(MyType&&) noexcept = default;
MyType& operator=(MyType&&) noexcept = default;
void swap(MyType& o) noexcept;
```

## Study Checklist
- [ ] Mark a custom class's move constructor `noexcept`; verify with `is_nothrow_move_constructible_v`
- [ ] Show the fallback to copy in `std::vector` reallocation when move is not `noexcept`
- [ ] Demonstrate that a throwing `noexcept` function calls `std::terminate`
- [ ] Use `noexcept(noexcept(expr))` to conditionally propagate noexcept

## References
- [cppreference — noexcept specifier](https://en.cppreference.com/w/cpp/language/noexcept_spec)
- *Effective Modern C++*, Scott Meyers — Item 14
