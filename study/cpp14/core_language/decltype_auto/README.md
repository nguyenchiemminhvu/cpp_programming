# C++14 › `decltype(auto)`

## Overview
`decltype(auto)` deduces a type using `decltype` rules (preserving references
and cv-qualifiers) rather than `auto` rules (which strip them). It bridges
the gap between "I want deduction" and "I need reference preservation".

## `auto` vs `decltype(auto)`
```cpp
int x = 0;
auto          f() { return x; }    // deduces int   — copy
decltype(auto) g() { return x; }   // deduces int   — copy (same here)
decltype(auto) h() { return (x); } // deduces int&  — REFERENCE (danger!)
// The extra parens make (x) an lvalue expression → decltype → int&
```

## Critical Use Case — Perfectly Returning a Reference
```cpp
// Without decltype(auto): may silently return by value
template<typename Container>
auto getElement(Container& c, int i) { return c[i]; }   // always copies

// With decltype(auto): returns reference if operator[] returns reference
template<typename Container>
decltype(auto) getElement(Container& c, int i) { return c[i]; }
```

## The Double-Paren Trap
```cpp
decltype(auto) dangerous() {
    int x = 42;
    return (x);   // returns int& to local variable — UB!
}
```

## Study Checklist
- [ ] Show that `auto` return strips references; `decltype(auto)` preserves them
- [ ] Write a perfect-return proxy wrapper using `decltype(auto)`
- [ ] Demonstrate the `(x)` double-paren trap returning a dangling reference
- [ ] Use `decltype(auto)` for a generic container element accessor

## References
- [cppreference — decltype(auto)](https://en.cppreference.com/w/cpp/language/auto#Return_type_deduction)
- *Effective Modern C++*, Scott Meyers — Item 3
