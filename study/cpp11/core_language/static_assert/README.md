# C++11 › `static_assert` — Compile-Time Assertions

## Overview
`static_assert(condition, message)` triggers a **compile-time error** with a
diagnostic message if `condition` evaluates to `false`. It is the compile-time
equivalent of `assert()`.

## Syntax
```cpp
static_assert(condition, "diagnostic message");
// C++17: message is optional
static_assert(sizeof(int) == 4);
```

## Key Use Cases

| Use Case | Example |
|----------|---------|
| Platform assumptions | `static_assert(sizeof(void*) == 8, "Requires 64-bit");` |
| Type constraints (pre-concepts) | `static_assert(std::is_integral<T>::value, "T must be integral");` |
| Template parameter validation | `static_assert(N > 0, "N must be positive");` |
| Struct layout verification | `static_assert(offsetof(Msg, field) == 4, "Layout mismatch");` |

## `static_assert` vs `#error`
- `#error` fires unconditionally at the preprocessor level.
- `static_assert` evaluates expressions with full type information and can be template-parameter-dependent.

## Study Checklist
- [ ] Guard a class template with `static_assert` on its type parameter
- [ ] Assert struct sizes for a network-protocol message layout
- [ ] Combine with `<type_traits>` to enforce callable constraints
- [ ] Show that a failed `static_assert` stops compilation with the exact message

## References
- [cppreference — static_assert](https://en.cppreference.com/w/cpp/language/static_assert)
