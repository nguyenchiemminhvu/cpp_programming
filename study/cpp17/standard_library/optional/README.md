# C++17 › `std::optional<T>`

## Overview
`std::optional<T>` represents a value that **may or may not be present**,
replacing sentinel values (`-1`, `nullptr`, `""`) and exception-based
"no value" signalling with an explicit, type-safe container.

## Core API
```cpp
#include <optional>

std::optional<int> find(const std::vector<int>& v, int x) {
    for (int i = 0; i < v.size(); ++i)
        if (v[i] == x) return i;   // implicitly wraps
    return std::nullopt;           // empty optional
}

auto idx = find(vec, 42);
if (idx)                    // or: idx.has_value()
    use(*idx);              // or: idx.value()
int safe = idx.value_or(-1); // provide a default
```

## Construction
```cpp
std::optional<std::string> o1;             // empty
std::optional<std::string> o2{"hello"};    // contains "hello"
std::optional<std::string> o3 = std::nullopt; // empty
auto o4 = std::make_optional<std::string>(5, 'x'); // "xxxxx"
```

## Pitfalls
- Accessing an empty optional via `*` or `value()` without checking is **undefined behaviour** (`value()` throws `std::bad_optional_access`, `*` is UB).
- `optional<T&>` is **not allowed** — use `optional<std::reference_wrapper<T>>` instead.
- Storing large objects in optional may add size/alignment overhead.

## Study Checklist
- [ ] Refactor a function that returns `-1` as "not found" to use `optional`
- [ ] Chain optional results with `value_or` to provide safe defaults
- [ ] Use `optional` as a lazy-initialised member variable
- [ ] Benchmark `optional<int>` vs `int*` for optional pointer semantics

## References
- [cppreference — optional](https://en.cppreference.com/w/cpp/utility/optional)
