# C++11 › `std::ratio` — Compile-Time Rational Arithmetic

## Overview
`std::ratio<N, D>` represents the rational number N/D as a compile-time type,
primarily used by `<chrono>` to define duration units with zero runtime overhead.

## Syntax
```cpp
#include <ratio>

using half       = std::ratio<1, 2>;
using quarter    = std::ratio<1, 4>;

// Arithmetic (returns a new ratio type)
using sum = std::ratio_add<half, quarter>;    // 3/4
static_assert(sum::num == 3 && sum::den == 4);
```

## Predefined SI Ratios
```cpp
std::kilo    // 1000/1
std::milli   // 1/1000
std::micro   // 1/1000000
std::nano    // 1/1000000000

// Used by chrono:
using milliseconds = std::chrono::duration<long long, std::milli>;
```

## Compile-Time Operations
`std::ratio_add`, `std::ratio_subtract`, `std::ratio_multiply`, `std::ratio_divide`,
`std::ratio_equal`, `std::ratio_less`

## Study Checklist
- [ ] Build a `Distance<T, Unit>` type parameterised on a `std::ratio`
- [ ] Perform compile-time unit conversion using `ratio_divide`
- [ ] Verify SI prefix values with `static_assert`
- [ ] Show how `<chrono>` uses `ratio` for `duration_cast`

## References
- [cppreference — ratio](https://en.cppreference.com/w/cpp/numeric/ratio/ratio)
