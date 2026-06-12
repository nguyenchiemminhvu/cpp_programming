# C++20 › `std::numbers` — Mathematical Constants

## Overview
`<numbers>` provides standard mathematical constants as `inline constexpr`
variable templates, replacing non-standard `M_PI` and similar macros with
portable, type-safe, precision-selectable constants.

## Available Constants
```cpp
#include <numbers>
namespace num = std::numbers;

num::pi_v<double>         // π  ≈ 3.14159265358979…
num::e_v<double>          // e  ≈ 2.71828182845905…
num::sqrt2_v<double>      // √2 ≈ 1.41421356237310…
num::sqrt3_v<double>      // √3 ≈ 1.73205080756888…
num::phi_v<double>        // φ  ≈ 1.61803398874989… (golden ratio)
num::ln2_v<double>        // ln(2)   ≈ 0.693147…
num::ln10_v<double>       // ln(10)  ≈ 2.302585…
num::log2e_v<double>      // log₂(e)
num::log10e_v<double>     // log₁₀(e)
num::inv_pi_v<double>     // 1/π
num::egamma_v<double>     // Euler–Mascheroni constant γ ≈ 0.5772…
```

## `double` Aliases
```cpp
std::numbers::pi    // equivalent to pi_v<double>
std::numbers::e     // equivalent to e_v<double>
```

## Study Checklist
- [ ] Compute the area of a circle using `std::numbers::pi` vs `M_PI`; compare values
- [ ] Use `pi_v<float>` and `pi_v<long double>` in a generic numeric function
- [ ] Verify `static_assert(std::numbers::sqrt2_v<double> * std::numbers::sqrt2_v<double> ≈ 2.0)`
- [ ] Replace all `#define M_PI` occurrences in a file with `std::numbers::pi`

## References
- [cppreference — Mathematical constants](https://en.cppreference.com/w/cpp/numeric/constants)
