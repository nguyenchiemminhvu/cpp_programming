# C++17 › Parallel Algorithms (`std::execution`)

## Overview
C++17 adds execution policy parameters to most `<algorithm>` and `<numeric>`
functions, enabling multi-threading and SIMD vectorisation with a one-word change.

## Execution Policies (`<execution>`)
| Policy | Type | Meaning |
|--------|------|---------|
| `std::execution::seq` | `sequenced_policy` | Sequential — default behaviour |
| `std::execution::par` | `parallel_policy` | Multi-threaded |
| `std::execution::par_unseq` | `parallel_unsequenced_policy` | Multi-threaded + SIMD |
| `std::execution::unseq` | `unsequenced_policy` | SIMD only (C++20) |

## Usage
```cpp
#include <algorithm>
#include <execution>
#include <numeric>

std::vector<int> v(10'000'000);
std::iota(v.begin(), v.end(), 0);

std::sort(std::execution::par_unseq, v.begin(), v.end());

auto sum = std::reduce(std::execution::par, v.begin(), v.end(), 0);
```

## Platform Notes
- Linux (GCC): requires `-ltbb` (Intel TBB).
- macOS: limited support; `par` may fall back to `seq` without TBB.
- MSVC: built-in support.

## Study Checklist
- [ ] Sort 10M elements sequentially vs `par`; measure wall-clock time
- [ ] Use `std::transform` with `par` on a compute-heavy operation
- [ ] Show a data race using `par` with a non-thread-safe accumulator
- [ ] Use `std::reduce` (order-independent) instead of `std::accumulate` for `par`

## References
- [cppreference — Execution policies](https://en.cppreference.com/w/cpp/algorithm/execution_policy_tag_t)
