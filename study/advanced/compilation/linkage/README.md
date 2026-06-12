# Advanced › Linkage

## Overview
**Linkage** determines which declarations refer to the same entity across
translation units. It controls name visibility at the linker level.

## Linkage Categories
| Linkage | Meaning | Example |
|---------|---------|---------|
| **External** | Visible across TUs — same entity everywhere | Global function, non-const global var |
| **Internal** | Visible only in this TU | `static` function/var, `namespace {}` entity |
| **No linkage** | Local to a scope | Local variables, `typedef` names |
| **Module linkage** (C++20) | Visible within a module | Non-exported module entities |

## Controlling Linkage
```cpp
static void f();            // internal linkage
namespace { void g(); }     // internal linkage (preferred over static for functions)
extern int x;               // external linkage declaration
inline void h() { }         // external linkage, ODR-exempt definition in header
```

## `inline` Functions and Variables
`inline` entities have **external linkage** but are **ODR-exempt**: multiple
definitions in different TUs are allowed as long as they are identical.

## `constexpr` Implies `inline` (C++17)
Namespace-scope `constexpr` variables are implicitly `inline` in C++17,
making them safe to define in headers.

## Study Checklist
- [ ] Show a name collision between two TUs; fix with internal linkage
- [ ] Explain the `inline` ODR exemption for header-defined functions
- [ ] Compare `static` vs anonymous namespace for internal linkage
- [ ] Show that `constexpr int N = 5;` in a header is safe in C++17

## References
- [cppreference — Storage class specifiers (linkage)](https://en.cppreference.com/w/cpp/language/storage_duration#Linkage)
