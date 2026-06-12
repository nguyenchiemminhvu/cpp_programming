# Design Pattern › CRTP — Curiously Recurring Template Pattern

## Overview
CRTP is a C++ idiom where a class `Derived` inherits from a base template
`Base<Derived>`. This gives the base class access to the derived type at
compile time, enabling **static polymorphism** — virtual-dispatch-like behaviour
with zero runtime overhead.

## Basic Structure
```cpp
template<typename Derived>
class Base {
public:
    void interface() {
        static_cast<Derived*>(this)->implementation();  // static dispatch
    }
    // Default implementation (optional)
    void implementation() { /* default */ }
};

class Concrete : public Base<Concrete> {
public:
    void implementation() { std::cout << "Concrete\n"; }
};

Concrete c;
c.interface();   // calls Concrete::implementation — no virtual dispatch
```

## Common Applications

### 1. Static Interface (no vtable)
Replace virtual methods with CRTP for zero-overhead polymorphism.

### 2. Mixin via CRTP
```cpp
template<typename Derived>
class Comparable {
    bool operator<(const Derived& o) const {
        return static_cast<const Derived*>(this)->compare(o) < 0;
    }
    bool operator>(const Derived& o) const { return o < *this; }
};

class Version : public Comparable<Version> {
    int compare(const Version& o) const { /* ... */ return 0; }
};
```

### 3. Fluent Builder
Return `*static_cast<Derived*>(this)` from base methods to allow chaining
through derived types.

## Study Checklist
- [ ] Implement a CRTP-based `Printable<T>` mixin
- [ ] Compare generated assembly: CRTP static dispatch vs virtual dispatch
- [ ] Build a CRTP construction counter tracking instances per derived type
- [ ] Show the pitfall: CRTP with a wrong derived type is hard-to-debug UB

## References
- [cppreference — CRTP](https://en.cppreference.com/w/cpp/language/crtp)
- *C++ Template Metaprogramming*, Abrahams & Gurtovoy
