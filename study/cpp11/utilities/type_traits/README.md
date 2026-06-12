# C++11 › `<type_traits>` — Compile-Time Type Introspection

## Overview
`<type_traits>` provides a metaprogramming toolkit for querying and transforming
types at compile time. It is the foundation of generic programming and SFINAE.

## Trait Categories

### Type queries (yield `std::true_type` / `std::false_type`)
| Trait | Asks |
|-------|------|
| `std::is_integral<T>` | Is `T` an integer type? |
| `std::is_floating_point<T>` | Is `T` float/double? |
| `std::is_pointer<T>` | Is `T` a pointer? |
| `std::is_class<T>` | Is `T` a class/struct? |
| `std::is_same<T,U>` | Are `T` and `U` identical? |
| `std::is_base_of<B,D>` | Does `D` derive from `B`? |
| `std::is_constructible<T,Args…>` | Can `T` be constructed from `Args`? |

### Type transformations (yield a nested `::type`)
| Trait | Effect |
|-------|--------|
| `std::remove_const<T>` | Strips top-level `const` |
| `std::remove_reference<T>` | Strips `&` or `&&` |
| `std::decay<T>` | Applies array-to-pointer, function-to-pointer, remove cv-ref |
| `std::add_pointer<T>` | Adds `*` |
| `std::enable_if<B,T>` | `::type = T` if `B` is true, else SFINAE |

### C++14 `_t` / `_v` aliases (preview)
`std::remove_const_t<T>` instead of `typename std::remove_const<T>::type`

## Study Checklist
- [ ] Use `static_assert` + `is_integral` to constrain a function template
- [ ] Write an overload set selected via `std::enable_if`
- [ ] Demonstrate `std::decay` on array and function types
- [ ] Build a compile-time type dispatch: different code paths for integral vs floating-point
- [ ] Predict the result of `std::is_same<int, int&&>::value`

## References
- [cppreference — type_traits](https://en.cppreference.com/w/cpp/header/type_traits)
- *Effective Modern C++*, Scott Meyers — Item 9
