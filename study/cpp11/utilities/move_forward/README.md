# C++11 › `std::move` & `std::forward`

## Overview
`std::move` and `std::forward` are cast utilities in `<utility>` that are
essential to move semantics and perfect forwarding. They do **not** move or
forward anything themselves — they only cast.

## `std::move`
```cpp
template<typename T>
constexpr std::remove_reference_t<T>&& move(T&& t) noexcept;
```
- Unconditionally casts its argument to an rvalue reference.
- Signals "you may steal the resources of this object".
- After a move, the object is in a **valid but unspecified** state.

## `std::forward`
```cpp
template<typename T>
constexpr T&& forward(std::remove_reference_t<T>& t) noexcept;
```
- Conditionally casts to rvalue or lvalue based on the deduced template argument `T`.
- Used exclusively with **forwarding references** (`T&&` in a deduced context).
- Preserves the value category of the original argument through a call chain.

## Mental Model
```
std::move(x)       // always → rvalue ref
std::forward<T>(x) // lvalue if T is lvalue ref type, rvalue otherwise
```

## Study Checklist
- [ ] Show that `std::move` on a `const` object still calls the copy constructor
- [ ] Write a variadic perfect-forwarding factory function (`emplace`-style)
- [ ] Demonstrate that double-move leads to use-after-move
- [ ] Trace the reference collapsing rules for `T&&` with lvalue and rvalue arguments
- [ ] Implement a simplified `std::make_pair` using `std::forward`

## References
- [cppreference — std::move](https://en.cppreference.com/w/cpp/utility/move)
- [cppreference — std::forward](https://en.cppreference.com/w/cpp/utility/forward)
- *Effective Modern C++*, Scott Meyers — Items 23, 25, 28
