# C++14 › `std::exchange`

## Overview
`std::exchange(obj, new_val)` assigns `new_val` to `obj` and returns the
**old value** of `obj`. It is the idiomatic way to implement move constructors
and to atomically get-and-reset state.

## Signature
```cpp
template<typename T, typename U = T>
T exchange(T& obj, U&& new_val);
// Equivalent to: auto old = std::move(obj); obj = std::forward<U>(new_val); return old;
```

## Move Constructor Pattern
```cpp
struct Buffer {
    int*   data_;
    size_t size_;

    Buffer(Buffer&& other) noexcept
        : data_{std::exchange(other.data_, nullptr)}
        , size_{std::exchange(other.size_, 0)}
    {}
};
```

## Toggle / Reset Pattern
```cpp
bool was_active = std::exchange(active_, false);   // get-and-clear
int  old_value  = std::exchange(counter_, 0);      // get-and-reset
```

## Study Checklist
- [ ] Implement a move constructor for a resource-owning class using `exchange`
- [ ] Compare to manual `auto old = x; x = new_val; return old;` — identical semantics
- [ ] Use in a state machine to atomically read-and-reset a flag
- [ ] Combine with `std::move` for efficient move-and-replace of large objects

## References
- [cppreference — exchange](https://en.cppreference.com/w/cpp/utility/exchange)
