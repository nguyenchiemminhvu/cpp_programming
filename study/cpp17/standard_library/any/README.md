# C++17 › `std::any`

## Overview
`std::any` is a **type-safe container for a single value of any type**. Unlike
`void*`, it retains type information and throws `std::bad_any_cast` on incorrect
type retrieval.

## Core API
```cpp
#include <any>

std::any a = 42;              // stores int
a = std::string{"hello"};    // stores string — old value destroyed

// Type query
std::cout << a.type().name(); // "NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE"

// Retrieval
std::string s = std::any_cast<std::string>(a);    // copy; throws if wrong type
std::string* ps = std::any_cast<std::string>(&a); // pointer; nullptr if wrong type

a.reset();         // empties the any
a.has_value();     // false
```

## Small Buffer Optimisation
Implementations typically use SBO: small objects are stored inline, avoiding
a heap allocation. Large objects are heap-allocated.

## When to Use `any` vs `variant` vs `void*`
| | `std::any` | `std::variant` | `void*` |
|--|------------|---------------|---------|
| Type set | Open (any type) | Closed (fixed set) | Open |
| Type safety | Yes (throws on bad cast) | Yes | No |
| Overhead | Higher (type-erasure) | Low | None |

## Study Checklist
- [ ] Store heterogeneous data in a `vector<std::any>` and retrieve each element
- [ ] Use `any_cast` with pointer form to avoid exceptions
- [ ] Compare `std::any` vs `std::variant` for a discriminated union use-case
- [ ] Measure the heap allocation behaviour of `std::any` with large vs small types

## References
- [cppreference — any](https://en.cppreference.com/w/cpp/utility/any)
