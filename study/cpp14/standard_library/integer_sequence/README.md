# C++14 › `std::integer_sequence`

## Overview
`std::integer_sequence<T, Ns...>` is a compile-time sequence of integers used
to unpack tuples, implement `std::apply`, and perform index-based template
metaprogramming.

## Key Aliases
```cpp
std::index_sequence<0,1,2>          // alias for integer_sequence<size_t, 0,1,2>
std::make_index_sequence<N>         // generates 0, 1, …, N-1
std::make_integer_sequence<T, N>    // generates 0, 1, …, N-1 of type T
```

## Canonical Pattern — Tuple-to-Args Expansion
```cpp
template<typename Tuple, std::size_t... I>
void print_tuple_impl(const Tuple& t, std::index_sequence<I...>) {
    ((std::cout << std::get<I>(t) << " "), ...);  // C++17 fold
}

template<typename... Ts>
void print_tuple(const std::tuple<Ts...>& t) {
    print_tuple_impl(t, std::make_index_sequence<sizeof...(Ts)>{});
}
```

## Study Checklist
- [ ] Implement a simplified `std::apply` using `make_index_sequence`
- [ ] Call a variadic function with each element of a tuple as an individual argument
- [ ] Use `index_sequence` to build a compile-time array of squared indices
- [ ] Compare the pre-C++14 manual index-pack idiom vs `make_index_sequence`

## References
- [cppreference — integer_sequence](https://en.cppreference.com/w/cpp/utility/integer_sequence)
