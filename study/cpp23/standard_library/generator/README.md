# C++23 › `std::generator<T>` — Coroutine Generator

## Overview
C++20 introduced coroutines but no ready-to-use coroutine types. C++23
(**P2502**) fills the biggest gap with `std::generator<Ref, Value, Allocator>`
in `<generator>` — a standardised **synchronous, lazy generator** that
integrates with the ranges library.

## Basic Usage
```cpp
#include <generator>

std::generator<int> iota(int from, int to) {
    for (int i = from; i < to; ++i)
        co_yield i;
}

for (int x : iota(0, 5))
    std::print("{} ", x);       // 0 1 2 3 4
```
`std::generator<T>` **satisfies `std::ranges::input_range`**, so it plugs
into every range algorithm and view.

## Template Parameters

| Parameter | Meaning |
|---|---|
| `Ref` | Reference type yielded by iteration (default `T`, `T&&` yielded as prvalue). |
| `Value` | Value type of the range (defaults to `remove_cvref_t<Ref>`). |
| `Allocator` | Allocator for the coroutine frame (default: dynamic). |

Use `std::generator<T&>` for a **reference-yielding** generator (no copy per
element).

## Recursive Generators — `co_yield elements_of(...)`
```cpp
std::generator<int> flatten(auto& tree) {
    for (auto& child : tree.children)
        co_yield std::ranges::elements_of(flatten(child));
    co_yield tree.value;
}
```
`elements_of` yields another range's items without an intermediate copy or
manual `for … co_yield` loop, and is efficient even for nested recursion.

## Integration With Views
```cpp
auto squares = iota(0, 100)
             | std::views::filter([](int i){ return i % 2 == 0; })
             | std::views::transform([](int i){ return i * i; });

for (int x : squares) …
```

## Ownership and Lifetime
`std::generator` **owns** the coroutine frame; the object is move-only. It is
safe to store as a member, pass by value, or use as a return type. When the
generator is destroyed mid-iteration, the coroutine is unwound cleanly.

## Allocation
By default the coroutine frame is heap-allocated. To customise:
```cpp
std::generator<int, int, MyAllocator<std::byte>> gen(MyAllocator<std::byte>{});
```
Some compilers can elide the allocation (HALO — Heap Allocation eLision
Optimisation) when the generator is used purely locally.

## Not Supported
- Async operations (`co_await something_async` — use `std::execution` /
  `boost::cobalt` / `libcoro` instead).
- Multi-threaded consumption of a single generator instance — a generator is
  single-consumer.

## Availability
- GCC 14+
- Clang: not yet in libc++ as of late 2025.
- MSVC 19.43+ (VS 2022 17.13)

## Study Checklist
- [ ] Rewrite a hand-rolled coroutine generator from C++20 in one line with `std::generator`
- [ ] Yield references (`std::generator<T&>`) and observe zero-copy iteration
- [ ] Use `co_yield std::ranges::elements_of(sub_gen)` in a tree flattener
- [ ] Pipe the generator through `std::views::filter` / `transform`
- [ ] Measure whether HALO elides the heap allocation for a local generator

## References
- [P2502 — `std::generator`: synchronous coroutine generator for ranges](https://wg21.link/P2502)
- [cppreference — `std::generator`](https://en.cppreference.com/w/cpp/coroutine/generator)
