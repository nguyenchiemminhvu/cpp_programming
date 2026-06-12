# C++11 › `auto` — Type Deduction

## Overview
`auto` instructs the compiler to deduce a variable's type from its initializer,
eliminating redundant type names and preventing implicit narrowing conversions.

## Key Concepts

| Construct | Deduced type |
|-----------|-------------|
| `auto x = 42;` | `int` |
| `auto x = 42.0;` | `double` |
| `auto& r = v;` | `T&` — preserves reference |
| `const auto& cr = v;` | `const T&` — read-only reference |
| `auto it = v.begin();` | iterator type |
| Trailing return `auto f() -> int` | explicit return via trailing syntax |

## Pitfalls
- `auto` **strips** top-level `const` and references. Write `auto&` or `const auto&` explicitly.
- `auto x{1};` → `int` (C++17 rule); `auto x = {1};` → `std::initializer_list<int>` (trap in C++11/14).
- Avoid `auto` when the deduced type is non-obvious at the call site; clarity beats brevity.

## Study Checklist
- [ ] Replace explicit iterator types in a loop with `auto`
- [ ] Understand cv-qualifier stripping: `const int ci = 0; auto x = ci;` — what is `x`?
- [ ] Use `auto&&` (forwarding reference) and compare with `auto&`
- [ ] Use `auto` in a trailing-return-type function template
- [ ] Demonstrate the `initializer_list` trap

## References
- [cppreference — auto](https://en.cppreference.com/w/cpp/language/auto)
- *Effective Modern C++*, Scott Meyers — Items 2, 5, 6
