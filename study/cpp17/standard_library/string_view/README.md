# C++17 › `std::string_view`

## Overview
`std::string_view` is a **non-owning, read-only view** into a contiguous
sequence of characters. It avoids the cost of constructing a `std::string`
when only read access is needed.

## Key Properties
- Stores a `const char*` pointer + length — **no heap allocation**.
- Implicitly constructable from `std::string`, `const char*`, `char[]`, and string literals.
- Does **not** own the underlying characters; the caller must ensure the data outlives the view.

## Core API
```cpp
#include <string_view>

void print(std::string_view sv) {    // accepts string, char*, literal — zero copy
    std::cout << sv.substr(0, 5);    // returns string_view, no allocation
    std::cout << sv.find("cpp");     // same interface as std::string
}

std::string s = "hello world";
std::string_view sv{s};             // view into s
sv.remove_prefix(6);                // sv == "world" — O(1)
```

## When to Use `string_view` vs `string`
| Use `string_view` when… | Use `string` when… |
|-------------------------|-------------------|
| Read-only parameter | Need to store / modify |
| Performance-critical hot path | Lifetime management needed |
| Span over substring without copy | Converting to `const char*` with NUL terminator |

## Pitfalls
- **Dangling view**: `string_view sv = std::string{"temp"};` — the temporary is destroyed immediately.
- `string_view` is **not NUL-terminated** — do not pass `.data()` to C functions expecting `const char*`.
- Returning `string_view` from a function is safe only if referencing a longer-lived buffer.

## Study Checklist
- [ ] Refactor all `const std::string&` function parameters to `std::string_view`
- [ ] Parse a log line using `string_view::find` and `substr` — zero allocations
- [ ] Demonstrate the dangling-view pitfall and how to avoid it
- [ ] Benchmark `string_view` vs `string` copy in a hot path

## References
- [cppreference — string_view](https://en.cppreference.com/w/cpp/string/basic_string_view)
