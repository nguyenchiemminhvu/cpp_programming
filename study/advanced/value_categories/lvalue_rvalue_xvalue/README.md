# Advanced › Value Categories: lvalue, rvalue, xvalue, glvalue, prvalue

## Overview
Every C++ expression has a **type** and a **value category**. Understanding
value categories is essential for mastering move semantics, overload resolution,
and the C++ object model.

## The Taxonomy (C++11)
```
        expression
       /          \
  glvalue        rvalue
  /     \       /     \
lvalue  xvalue       prvalue
```

| Category | Has identity? | Movable? | Examples |
|----------|--------------|---------|----------|
| **lvalue** | Yes | No | Variable names, `*ptr`, `a.member` |
| **xvalue** | Yes | Yes | `std::move(x)`, function returning `T&&` |
| **prvalue** | No | Yes | `42`, `T{}`, lambda |
| **glvalue** | Yes | — | lvalue + xvalue |
| **rvalue** | — | Yes | xvalue + prvalue |

## Key Examples
```cpp
int x = 5;
int& lref  = x;              // lvalue reference — x is lvalue
int&& rref = std::move(x);   // rvalue reference binds to xvalue

// Function returning T&& — produces xvalue
std::string&& get() { static std::string s; return std::move(s); }
```

## Why It Matters
- Overload resolution picks `T&&` over `const T&` for rvalues.
- Move semantics only apply to rvalues (xvalue + prvalue).
- C++17 prvalue materialisation changes when copies are elided.

## Study Checklist
- [ ] Classify 10 expressions as lvalue, xvalue, or prvalue
- [ ] Show that `std::move(x)` produces an xvalue (identity + movable)
- [ ] Demonstrate overload resolution preferring `T&&` over `const T&` for rvalues
- [ ] Explain why `T&& = T{}` is valid but `T& = T{}` is not

## References
- [cppreference — Value categories](https://en.cppreference.com/w/cpp/language/value_category)
