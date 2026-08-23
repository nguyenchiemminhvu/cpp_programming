# C++23 › `std::unreachable`

## Overview
C++23 (**P0627**) adds `std::unreachable()` in `<utility>` — a portable
counterpart to the compiler builtins `__builtin_unreachable` (GCC/Clang) and
`__assume(false)` (MSVC). Calling it tells the optimiser that this program
point can never be reached; reaching it at runtime is **undefined behaviour**.

## Signature
```cpp
namespace std {
    [[noreturn]] void unreachable();
}
```
No parameters, no return, `noreturn` attribute, `constexpr` in some
implementations for use in constant-evaluation dead branches.

## Canonical Use — Exhaustive Switch
```cpp
enum class op { add, sub, mul };

int apply(op o, int a, int b) {
    switch (o) {
        case op::add: return a + b;
        case op::sub: return a - b;
        case op::mul: return a * b;
    }
    std::unreachable();   // any other value is a bug
}
```
The optimiser will typically drop the epilogue after the switch and generate
a simple jump table.

## `std::unreachable` vs `[[assume(false)]]`
| Feature | `std::unreachable()` | `[[assume(false)]]` |
|---|---|---|
| Kind | Function call | Attribute on a statement |
| Semantics | Reaching UB | Compiler may assume UB not reached |
| Attribute form | `[[noreturn]]` | statement attribute |
| Typical placement | End of function, after exhaustive switch | Inside a branch as a "cannot happen" note |
| Portability | Standard C++23 header `<utility>` | Standard C++23 language attribute |

Both convey the same UB contract; pick whichever reads better in context.
`std::unreachable()` composes with `[[noreturn]]` warnings ("all paths return
a value"); `[[assume(false)]]` does not.

## `std::unreachable` vs `assert(false)`
- `assert(false)` **traps in debug**, **silently vanishes in release** — often
  desired for tests.
- `std::unreachable()` is UB *always* — do not use it to catch bugs. Prefer
  `assert(false)` (or a hardened alternative) for logic errors that might
  actually happen, and `std::unreachable()` only for provably impossible cases.

Debug-safe hybrid:
```cpp
#ifdef NDEBUG
    std::unreachable();
#else
    std::abort();
#endif
```
Or, using the C++23 attribute:
```cpp
assert(!"impossible");
[[assume(false)]];
```

## Study Checklist
- [ ] Add `std::unreachable()` after an exhaustive `switch` — inspect the assembly
- [ ] Compare compile-time behaviour with `[[assume(false)]]`
- [ ] Reach `std::unreachable()` at runtime deliberately — observe UB
      (crash, garbage, or nasal demons — do NOT ship this)
- [ ] Write a `switch_exhaustive!` macro that ends with `std::unreachable()`
- [ ] Contrast with `assert(false)` — when is each right?

## References
- [P0627 — Function to mark unreachable code](https://wg21.link/P0627)
- [cppreference — `std::unreachable`](https://en.cppreference.com/w/cpp/utility/unreachable)
