# Advanced › Full Template Specialisation

## Overview
Full template specialisation provides a completely different implementation
for a specific set of template arguments, overriding the primary template.

## Syntax
```cpp
// Primary template
template<typename T>
struct Serialiser {
    static void write(const T& v) { /* generic implementation */ }
};

// Full specialisation for bool
template<>
struct Serialiser<bool> {
    static void write(bool v) { std::cout << (v ? "true" : "false"); }
};

// Full specialisation for a function template
template<typename T> T max(T a, T b) { return a > b ? a : b; }
template<> const char* max<const char*>(const char* a, const char* b) {
    return std::strcmp(a, b) > 0 ? a : b;
}
```

## Rules
- Function template specialisations do **not** participate in overload resolution
  like regular functions — prefer overloads for functions, specialisations for class templates.
- The specialisation must be declared **after** the primary template.
- Static data members of a specialised class must be defined separately.

## Study Checklist
- [ ] Specialise `std::hash<T>` for a custom type
- [ ] Show that a class template full specialisation can have different member sets
- [ ] Demonstrate why function template specialisation ordering can be surprising
- [ ] Implement a `TypeName<T>` trait with specialisations for all built-in types

## References
- [cppreference — Template specialization](https://en.cppreference.com/w/cpp/language/template_specialization)
