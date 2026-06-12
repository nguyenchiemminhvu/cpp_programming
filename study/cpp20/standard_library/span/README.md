# C++20 › `std::span<T>`

## Overview
`std::span<T>` is a **non-owning, contiguous view** over a sequence of `T`
elements. It generalises `T*` + size into a safe, typed boundary-aware wrapper
usable with arrays, `std::vector`, `std::array`, and raw pointers.

## Variants
| Form | Description |
|------|-------------|
| `std::span<T>` | Dynamic extent — size stored at runtime |
| `std::span<T, N>` | Static extent — size known at compile time, no runtime overhead |

## Core API
```cpp
#include <span>

void process(std::span<const int> data) {
    for (int x : data)
        std::cout << x << " ";
    std::cout << data.size() << "\n";
    auto first_half = data.first(data.size() / 2);   // subspan, no copy
    auto second_half = data.last(data.size() / 2);
}

std::vector<int> v{1, 2, 3, 4, 5};
process(v);          // implicit conversion

int arr[] = {1, 2, 3};
process(arr);        // also works
```

## `span` vs `string_view`
- `string_view` = `span<const char>` + string-specific methods.
- `span<T>` is mutable by default (use `span<const T>` for read-only).

## Safety Note
Like `string_view`, `span` does **not** own its data. Always ensure the
underlying container outlives the span.

## Study Checklist
- [ ] Replace `(T* data, size_t len)` function parameters with `std::span<T>`
- [ ] Use `span::subspan`, `first`, `last` to slice without copying
- [ ] Use `std::span<T, N>` for a compile-time fixed-size array view
- [ ] Pass a `span<const int>` to a function accepting different container types
- [ ] Show a dangling span and how to avoid it

## References
- [cppreference — span](https://en.cppreference.com/w/cpp/container/span)
