# C++11 › `<random>` — Random Number Generation

## Overview
`<random>` provides a modern, composable random number generation system that
separates the **engine** (source of randomness) from the **distribution**
(mapping to a range), replacing `rand()` / `srand()`.

## Canonical Pattern
```cpp
#include <random>

std::mt19937 rng{std::random_device{}()};       // seed from hardware entropy
std::uniform_int_distribution<int> dist{1, 6};  // [1, 6] inclusive

int roll = dist(rng);
```

## Engines
| Engine | Notes |
|--------|-------|
| `std::mt19937` | Mersenne Twister 32-bit — fast, good quality |
| `std::mt19937_64` | 64-bit variant |
| `std::default_random_engine` | Implementation-defined |
| `std::random_device` | Hardware entropy source (use for seeding only) |

## Distributions
`uniform_int_distribution`, `uniform_real_distribution`,
`normal_distribution`, `bernoulli_distribution`, `poisson_distribution`,
`exponential_distribution`

## Study Checklist
- [ ] Simulate 10 000 coin flips with `bernoulli_distribution`; verify ~50% heads
- [ ] Generate a normally distributed dataset with `normal_distribution<double>`
- [ ] Seed with a fixed value for reproducible unit test results
- [ ] Show why `rand()` is inferior: modulo bias, global state, thread-unsafety

## References
- [cppreference — random](https://en.cppreference.com/w/cpp/numeric/random)
