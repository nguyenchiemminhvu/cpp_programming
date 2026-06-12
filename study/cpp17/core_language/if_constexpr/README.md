# C++17 › `if constexpr`

## Overview
`if constexpr (condition)` evaluates the condition at **compile time** and
discards the unselected branch entirely — it is neither compiled nor
instantiated. This eliminates SFINAE and tag-dispatch boilerplate in templates.

## Syntax
```cpp
template<typename T>
void print(T val) {
    if constexpr (std::is_integral_v<T>) {
        std::cout << "int: " << val << "\n";
    } else if constexpr (std::is_floating_point_v<T>) {
        std::cout << "float: " << val << "\n";
    } else {
        std::cout << "other: " << val << "\n";
    }
}
```

## Key Properties
- The **discarded branch** is not instantiated → it can contain ill-formed code for unmatched types.
- Works only inside templates (or when the condition is a template-parameter-dependent expression).
- **Not** the same as `#if` — both branches must still be syntactically valid.
- Does **not** restrict the scope: variables declared in the taken branch are still in scope after the `if`.

## Common Patterns
```cpp
// Recursive variadic template termination
template<typename T, typename... Ts>
void log(T first, Ts... rest) {
    std::cout << first;
    if constexpr (sizeof...(rest) > 0)
        log(rest...);
}
```

## Study Checklist
- [ ] Replace a tag-dispatch overload set with a single `if constexpr` function
- [ ] Implement `to_string` for integral, floating-point, and string types
- [ ] Write a variadic logger using `if constexpr` for pack-expansion termination
- [ ] Show a case where the discarded branch would not compile for the given type

## References
- [cppreference — if constexpr](https://en.cppreference.com/w/cpp/language/if#Constexpr_if)
