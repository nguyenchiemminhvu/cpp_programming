# Advanced › Template Instantiation & ODR

## Overview
Understanding when and how templates are instantiated is critical for
controlling code size, compilation speed, and avoiding subtle ODR violations.

## Implicit vs Explicit Instantiation

### Implicit
The compiler generates an instance on first use:
```cpp
std::vector<int> v;   // implicitly instantiates vector<int>
```

### Explicit Instantiation Declaration (suppress in TU)
```cpp
// header.h
extern template class std::vector<int>;   // declaration: "defined elsewhere"

// instantiation.cpp
template class std::vector<int>;          // explicit definition — only here
```
Reduces compile times by instantiating a template in one TU only.

### Explicit Instantiation Definition (force specific overload)
```cpp
template void sort<int*>(int*, int*);   // force this specific instantiation
```

## Two-Phase Lookup
Template code is checked in two phases:
1. **Phase 1 (definition)**: Non-dependent names looked up.
2. **Phase 2 (instantiation)**: Dependent names looked up at the point of use.

```cpp
template<typename T>
void f(T x) {
    undeclared();       // Phase 1 error — non-dependent name
    x.method();         // Phase 2 — checked at instantiation
}
```

## Study Checklist
- [ ] Use `extern template` to reduce duplicate instantiations across TUs
- [ ] Show a phase-1 vs phase-2 lookup error with a concrete example
- [ ] Measure compile time: with/without explicit instantiation for a heavy template
- [ ] Explain when `template class Foo<Bar>` in multiple TUs causes an ODR violation

## References
- [cppreference — Template instantiation](https://en.cppreference.com/w/cpp/language/class_template#Explicit_instantiation)
