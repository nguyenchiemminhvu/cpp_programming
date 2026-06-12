# Advanced › Partial Template Specialisation

## Overview
Partial specialisation provides a customised implementation for a **subset**
of template arguments, keeping some parameters generic. Available for **class
templates** only (not function templates).

## Syntax
```cpp
// Primary template
template<typename T, typename U>
struct IsConvertible : std::false_type {};

// Partial specialisation: same type — always convertible
template<typename T>
struct IsConvertible<T, T> : std::true_type {};

// Partial specialisation for pointer types
template<typename T>
struct Storage<T*> {
    T* ptr = nullptr;
    T& operator*() { return *ptr; }
};
```

## Pointer / Reference Patterns
```cpp
template<typename T> struct Wrapper     { T  value; };   // primary
template<typename T> struct Wrapper<T*> { T* ptr;   };   // for pointers
template<typename T> struct Wrapper<T&> { T& ref;   };   // for references
template<>           struct Wrapper<void>{};              // full spec
```

## Function Templates: No Partial Specialisation
Function templates cannot be partially specialised — use overloading or
delegate to a helper class template.

## Study Checklist
- [ ] Partially specialise a `Print<T>` template for `std::vector<T>`
- [ ] Implement `RemovePointer<T*>` trait using partial specialisation
- [ ] Build a `TypeList` metaprogramming utility using partial specialisation
- [ ] Show why `std::is_pointer` is implemented with partial specialisation

## References
- [cppreference — Partial template specialization](https://en.cppreference.com/w/cpp/language/partial_specialization)
