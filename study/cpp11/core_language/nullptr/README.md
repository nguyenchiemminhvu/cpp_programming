# C++11 › `nullptr` — Type-Safe Null Pointer

## Overview
`nullptr` is a keyword of type `std::nullptr_t`, replacing the ambiguous `NULL`
macro (which is `0` or `(void*)0` in C) with an unambiguous, type-safe null
pointer constant.

## The Problem With `NULL`
```cpp
void f(int x);
void f(char* p);

f(NULL);    // C++03: calls f(int) — likely a bug
f(nullptr); // C++11: unambiguously calls f(char*)
```

## `std::nullptr_t`
- `nullptr` has type `std::nullptr_t`.
- Implicitly converts to **any pointer** or pointer-to-member type.
- Does **not** implicitly convert to integer types (except `bool`).
- `sizeof(nullptr_t) == sizeof(void*)`.

## Study Checklist
- [ ] Show the `NULL` overload-resolution ambiguity; fix with `nullptr`
- [ ] Pass `nullptr` to a function taking `std::unique_ptr<T>` and one taking `int*`
- [ ] Use `nullptr` in a template context; inspect the deduced type with `typeid`
- [ ] Write a function taking `std::nullptr_t` as a dedicated "no-value" overload

## References
- [cppreference — nullptr](https://en.cppreference.com/w/cpp/language/nullptr)
- *Effective Modern C++*, Scott Meyers — Item 8
