# C++20 › Requires Expressions

## Overview
A **requires expression** is a compile-time boolean expression that checks
whether a set of operations are valid for given types. It is the mechanism
behind concepts and can also be used inline.

## Syntax
```cpp
requires (parameter-list) { requirements; }
```

## Four Requirement Types

### Simple requirement — expression must be valid
```cpp
requires (T x) { x + x; }          // T must support operator+
```

### Type requirement — type must exist
```cpp
requires { typename T::value_type; }
```

### Compound requirement — expression is valid and its type satisfies a concept
```cpp
requires (T x) {
    { x.size() } -> std::convertible_to<std::size_t>;
}
```

### Nested requirement — additional constraint
```cpp
requires (T x) {
    requires std::integral<T>;
}
```

## Building a Concept from a Requires Expression
```cpp
template<typename T>
concept Container = requires(T c) {
    typename T::value_type;
    typename T::iterator;
    { c.begin() } -> std::input_iterator;
    { c.end()   } -> std::input_iterator;
    { c.size()  } -> std::convertible_to<std::size_t>;
};
```

## Inline Ad-hoc Requires Clause
```cpp
template<typename T>
    requires requires(T x) { x.serialize(); }
void save(T obj) { obj.serialize(); }
```

## Study Checklist
- [ ] Write a `Hashable` concept using a compound requirement
- [ ] Distinguish: named concept vs inline requires expression
- [ ] Show that requires expressions short-circuit on first failed requirement
- [ ] Combine `requires` clause + `requires` expression in one template

## References
- [cppreference — Requires expression](https://en.cppreference.com/w/cpp/language/requires)
