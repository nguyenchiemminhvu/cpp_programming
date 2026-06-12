# C++20 › `std::source_location`

## Overview
`std::source_location` (in `<source_location>`) provides compile-time information
about the caller's source location — file, line, column, function name —
replacing `__FILE__`, `__LINE__`, and `__func__` macros with a type-safe API.

## Core API
```cpp
#include <source_location>

void log(std::string_view msg,
         const std::source_location loc = std::source_location::current()) {
    std::cout << loc.file_name() << ':' << loc.line()
              << " [" << loc.function_name() << "]: " << msg << '\n';
}

log("something happened");   // records the call site, not log()'s own location
```

## Member Functions
| Function | Returns |
|----------|---------|
| `file_name()` | Source file path (`const char*`) |
| `line()` | Line number (1-based, `uint_least32_t`) |
| `column()` | Column number (implementation-defined) |
| `function_name()` | Enclosing function signature (`const char*`) |

## Study Checklist
- [ ] Build a logging utility that captures caller location via `source_location::current()`
- [ ] Compare output of `source_location` vs `__FILE__`/`__LINE__` macros
- [ ] Test in a template function — observe what `function_name()` returns
- [ ] Chain through helper functions: verify the default-argument captures the correct site

## References
- [cppreference — source_location](https://en.cppreference.com/w/cpp/utility/source_location)
