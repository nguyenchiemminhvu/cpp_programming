# C++23 › `[[assume(expr)]]` — Portable Optimiser Hint

## Overview
C++23 (**P1774**) adds a **portable** attribute that tells the optimiser it may
assume an expression holds without evaluating it. Historically this was
`__builtin_assume(x)` (Clang), `__assume(x)` (MSVC), or
`if (!x) __builtin_unreachable();` (GCC). The standard attribute replaces all
three.

## Syntax
```cpp
double sqrt_impl(double x) {
    [[assume(x >= 0.0)]];          // optimiser may skip negative-input branches
    return std::sqrt(x);
}
```

## Rules
- The expression is **not evaluated at runtime**. If it would have side effects
  or throw, they do **not** happen.
- Violating the assumption at runtime is **undefined behaviour** — treat it as
  a stricter form of `assert`.
- The expression must be *potentially-constant-evaluated*: no calls to
  non-constexpr virtual functions, no dependencies on non-trivially-copyable
  volatile objects, etc.
- The attribute may appear anywhere a statement-level attribute is legal.

## Idioms

### Loop bounds hint
```cpp
void process(std::span<int> buf) {
    [[assume(buf.size() % 4 == 0)]];   // enables 4-wide vectorisation
    for (auto& x : buf) x += 1;
}
```

### Enum exhaustiveness
```cpp
enum class op { add, sub, mul };
int apply(op o, int a, int b) {
    switch (o) {
        case op::add: return a + b;
        case op::sub: return a - b;
        case op::mul: return a * b;
    }
    [[assume(false)]];                 // no other values reach here
}
```
(For an *unconditional* unreachable, prefer `std::unreachable()` — see the
sibling folder `unreachable`. `[[assume(false)]]` is a weaker, attribute-form
equivalent.)

### Pointer alignment
```cpp
void memcpy_fast(void* dst, const void* src, std::size_t n) {
    [[assume(reinterpret_cast<std::uintptr_t>(dst) % 16 == 0)]];
    [[assume(reinterpret_cast<std::uintptr_t>(src) % 16 == 0)]];
    // …vectorised body
}
```

## Debug-Only Cross-Check
```cpp
#define ASSUME(x) do { assert(x); [[assume(x)]]; } while (0)
```
`assert` in debug builds catches violations; `[[assume]]` in release builds
tells the optimiser.

## Availability
- GCC 13+, Clang 19+, MSVC 19.32+
- Older compilers accept the attribute syntax under `[[msvc::assume]]`
  or ignore it as unknown (the standard requires *unknown* attributes to be
  ignored with only a warning).

## Study Checklist
- [ ] Apply `[[assume]]` to a hot loop and inspect the generated assembly
- [ ] Compare before/after codegen with `-O2` or `/O2`
- [ ] Combine with `assert` for a debug-verified assumption
- [ ] Confirm the expression is *not* executed at runtime (use a side-effecting
      dummy that would throw)
- [ ] Contrast `[[assume(false)]]` vs `std::unreachable()`

## References
- [P1774 — Portable assumptions](https://wg21.link/P1774)
- [cppreference — `[[assume]]`](https://en.cppreference.com/w/cpp/language/attributes/assume)
