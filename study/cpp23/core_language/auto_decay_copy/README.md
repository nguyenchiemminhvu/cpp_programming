# C++23 › `auto(x)` / `auto{x}` — Decay-Copy Expression

## Overview
C++23 (**P0849**) makes `auto(x)` and `auto{x}` valid *expressions* that produce
a **prvalue copy** of `x` with references stripped and arrays / functions
decayed — i.e. `std::decay_t<decltype(x)>`. This standardises the common idiom
of "give me a fresh, mutable copy that I own".

## Syntax
```cpp
void consume(std::string);

void handler(const std::string& msg) {
    consume(auto(msg));            // ✅ prvalue copy of `msg`
}
```
Equivalent to (but shorter and clearer than):
```cpp
consume(std::decay_t<decltype(msg)>(msg));
consume([](auto x){ return x; }(msg));      // the old trick
```

## `auto(x)` vs `auto{x}`
| Form | Behaviour |
|---|---|
| `auto(x)` | Direct-init decay-copy; allows narrowing |
| `auto{x}` | List-init decay-copy; rejects narrowing conversions |

Prefer `auto{x}` for safety unless a narrowing conversion is intended.

## Use Cases
### 1. Detach from an lvalue you want to modify
```cpp
template <typename Container>
void erase_duplicates(Container& c) {
    auto tmp = auto(c);            // fresh copy, independent lifetime
    std::ranges::sort(tmp);
    // ...
}
```

### 2. Break aliasing when calling an algorithm on itself
```cpp
std::vector<int> v = /*…*/;
std::ranges::copy(v, std::back_inserter(auto(v)));   // separate destination
```

### 3. Avoid dangling references in generic code
```cpp
template <typename T>
auto make_owned(const T& x) { return auto(x); }
```

### 4. Function-argument decay-copy at the call site
```cpp
launch_task(auto(config));         // pass a decayed copy, not a reference
```

## Not the Same As `T(x)`
`auto(x)` decays; a named-type functional cast like `std::string(msg)` does
not. `auto(x)` follows exactly the value-category and type rules of `decay_t`.

## Study Checklist
- [ ] Replace the "immediate lambda" trick `[](auto x){return x;}(v)` with `auto(v)`
- [ ] Try `auto{narrowing_conv}` and confirm the compiler rejects it
- [ ] Verify decay: `auto(arr)` on an array produces a pointer
- [ ] Use `auto(x)` inside a `constexpr` function
- [ ] Contrast with `std::forward` and `std::move` — when is each correct?

## References
- [P0849 — `auto(x)`: decay-copy in the language](https://wg21.link/P0849)
- [cppreference — `auto(x)` decay copy](https://en.cppreference.com/w/cpp/language/auto)
