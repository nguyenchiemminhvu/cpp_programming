# C++11 › Scoped Enumerations (`enum class`)

## Overview
`enum class` (and `enum struct`) creates a **scoped**, **strongly-typed**
enumeration. Enumerators do not pollute the enclosing scope and do not
implicitly convert to integers.

## `enum` vs `enum class`
```cpp
// Unscoped (C++03) — enumerators leak into enclosing scope
enum Color { Red, Green, Blue };
int x = Red;         // OK but risky; Red pollutes the namespace

// Scoped (C++11)
enum class Status { Active, Inactive, Pending };
Status s = Status::Active;    // must qualify
// int y = Status::Active;    // ERROR — no implicit conversion
int z = static_cast<int>(Status::Active);   // explicit cast required
```

## Underlying Type Control
```cpp
enum class Flags : uint8_t { A = 1, B = 2, C = 4 };
static_assert(sizeof(Flags) == 1);
```

## Study Checklist
- [ ] Replace a plain `enum` with `enum class`; fix all call sites
- [ ] Specify a `uint8_t` underlying type; verify `sizeof`
- [ ] Demonstrate that two `enum class` values with the same name coexist without collision
- [ ] Use `switch` over an `enum class` and enable compiler exhaustiveness warnings

## References
- [cppreference — Scoped enumerations](https://en.cppreference.com/w/cpp/language/enum#Scoped_enumerations)
- *Effective Modern C++*, Scott Meyers — Item 10
