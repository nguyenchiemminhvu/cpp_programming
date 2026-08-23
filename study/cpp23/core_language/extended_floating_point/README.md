# C++23 › Extended Floating-Point Types

## Overview
C++23 (**P1467**) introduces a family of **named, fixed-width floating-point
types** in `<stdfloat>`, mirroring `<cstdint>` for integers. These are optional
implementation-provided types with well-defined binary formats, chosen from
IEEE 754 and the machine-learning-inspired bfloat16 format.

## The Types

| Type | Binary format | Bits | Typical use |
|---|---|---|---|
| `std::float16_t` | IEEE 754 binary16 | 16 | Storage, GPU tensors |
| `std::float32_t` | IEEE 754 binary32 | 32 | Portable single-precision |
| `std::float64_t` | IEEE 754 binary64 | 64 | Portable double-precision |
| `std::float128_t` | IEEE 754 binary128 | 128 | Extended-precision scientific |
| `std::bfloat16_t` | brain-float 16 | 16 | ML training / inference |

All live in `<stdfloat>` and are conditionally present — check with feature
test macros:
```cpp
#include <stdfloat>
#if defined(__STDCPP_FLOAT16_T__)
    std::float16_t half = 3.14f16;
#endif
```

## Literal Suffixes

| Suffix | Type |
|---|---|
| `f16` | `float16_t` |
| `f32` | `float32_t` |
| `f64` | `float64_t` |
| `f128` | `float128_t` |
| `bf16` | `bfloat16_t` |

Example: `constexpr auto pi_f32 = 3.14159f32;`

## Conversion Rules
- Implicit conversions **only** among these named types follow value-preservation
  rules: narrowing conversions are ill-formed in list-initialisation.
- Interaction with `float` / `double` / `long double` is intentionally minimal —
  those keep their platform-specific meaning; the new types are the portable
  alternative.

## Why They Matter
- **Cross-platform reproducibility** — `float` might mean binary32 on x86
  but a different format on some DSPs; `std::float32_t` is exactly binary32.
- **ML / GPU interop** — `std::float16_t` and `std::bfloat16_t` finally have
  standard names, format, and printing rules.
- **Overload sets** — distinct types allow overloading a function on
  16-bit vs 32-bit precision:
  ```cpp
  void render(std::float16_t px);
  void render(std::float32_t px);   // distinct overload
  ```

## Support
Compiler / library support is very uneven at the time of writing:
- GCC 13+ supports `float16_t`, `float32_t`, `float64_t`, `bfloat16_t`
  when the target ABI defines them.
- MSVC / libc++ support is arriving incrementally.
- Always guard with `__STDCPP_FLOAT<n>_T__` feature-test macros.

## Study Checklist
- [ ] Include `<stdfloat>` and verify which named types are available
- [ ] Write literals for each type and check `sizeof` matches expectations
- [ ] Overload a function on `float32_t` vs `float64_t` and observe selection
- [ ] Convert between `float16_t` and `bfloat16_t` explicitly; note the precision loss
- [ ] Read/write a `float16_t` array to a binary file and re-read on another platform

## References
- [P1467 — Extended floating-point types and standard names](https://wg21.link/P1467)
- [cppreference — `<stdfloat>`](https://en.cppreference.com/w/cpp/header/stdfloat)
