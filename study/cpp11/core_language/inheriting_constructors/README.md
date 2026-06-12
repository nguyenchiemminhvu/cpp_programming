# C++11 › Inheriting Constructors

## Overview
`using Base::Base;` makes all constructors of `Base` available in a derived
class, eliminating boilerplate forwarding constructors in thin wrappers.

## Syntax
```cpp
class Base {
public:
    Base(int x)            { }
    Base(int x, double y)  { }
    Base(std::string name) { }
};

class Derived : public Base {
public:
    using Base::Base;  // inherits all three Base constructors
};

Derived d1{42};
Derived d2{1, 3.14};
Derived d3{"hello"};
```

## Important Nuances
- Inherited constructors keep their `explicit` specifier if present.
- Default, copy, and move constructors are **not** inherited.
- If `Derived` declares a constructor with the same signature, it **hides** the inherited one.
- Members of `Derived` without a default member initialiser cause problems.

## Study Checklist
- [ ] Create a `LoggedVector` that inherits all `std::vector` constructors
- [ ] Show that adding a member without a default initializer breaks it
- [ ] Demonstrate that copy/move constructors are NOT inherited
- [ ] Combine inheriting constructors with delegating constructors in `Derived`

## References
- [cppreference — Using declaration (inheriting constructors)](https://en.cppreference.com/w/cpp/language/using_declaration#Inheriting_constructors)
