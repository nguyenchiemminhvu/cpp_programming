# C++11 › Reference Collapsing Rules

## Overview
Reference collapsing is the language rule that determines the resulting
reference type when references are composed — it underpins forwarding
references, `std::forward`, and `std::move`.

## The Rule
> A reference to a reference collapses to a single reference.
> **Only `&&` + `&&` → `&&`; everything else → `&`.**

| Template arg `T` | Parameter `T&&` | Result |
|-----------------|-----------------|--------|
| `int` (rvalue) | `int&&` | `int&&` |
| `int&` | `int& &&` | `int&` |
| `int&&` | `int&& &&` | `int&&` |
| `const int&` | `const int& &&` | `const int&` |

## Where It Applies
1. **Forwarding references** — `template<typename T> f(T&& x)`
2. **`typedef` / `using` aliases** — `using Ref = int&; Ref& r = …;` → `int&`
3. **`decltype`** expressions
4. **`std::forward` implementation** — returns `T&&` which collapses correctly

## `std::move` and `std::forward` Decoded
```cpp
// std::move — always produces rvalue ref
template<typename T>
remove_reference_t<T>&& move(T&& t) {
    return static_cast<remove_reference_t<T>&&>(t);
}

// std::forward — conditional; collapses via T
template<typename T>
T&& forward(remove_reference_t<T>& t) {
    return static_cast<T&&>(t);
}
```

## Study Checklist
- [ ] Manually trace reference collapsing for 5 different `T` + `T&&` combinations
- [ ] Explain why `std::forward<T>(x)` is identity when `T` is an lvalue ref
- [ ] Show `using Alias = int&; Alias&& r = …;` — what is `r`?
- [ ] Implement a simplified `forward` using only reference collapsing

## References
- [cppreference — Reference declaration](https://en.cppreference.com/w/cpp/language/reference)
- *Effective Modern C++*, Scott Meyers — Item 28
