# C++11 › `<chrono>` — Time Utilities

## Overview
`<chrono>` provides a type-safe time library built on three orthogonal concepts:
**clocks**, **time points**, and **durations**. It replaces `<ctime>` for most
use cases with compile-time unit checking.

## Three Core Concepts

### Clocks
| Clock | Use |
|-------|-----|
| `std::chrono::system_clock` | Wall-clock time; convertible to `time_t` |
| `std::chrono::steady_clock` | Monotonic; use for measuring intervals |
| `std::chrono::high_resolution_clock` | Highest resolution available (may alias either above) |

### Durations
```cpp
using namespace std::chrono;
seconds s{1};
milliseconds ms{500};
auto total = s + ms;  // 1500ms — units are type-safe
```
Standard duration aliases: `hours`, `minutes`, `seconds`, `milliseconds`, `microseconds`, `nanoseconds`.

### Time Points
```cpp
auto start = steady_clock::now();
// ... work ...
auto elapsed = duration_cast<milliseconds>(steady_clock::now() - start);
```

## Study Checklist
- [ ] Write a `Stopwatch` RAII class using `steady_clock`
- [ ] Convert between duration units with `duration_cast`
- [ ] Format a `system_clock::time_point` to a human-readable string via `time_t`
- [ ] Measure the cost of a function call over 1 million iterations
- [ ] Use `std::this_thread::sleep_for` with a `chrono` duration

## References
- [cppreference — chrono](https://en.cppreference.com/w/cpp/chrono)
