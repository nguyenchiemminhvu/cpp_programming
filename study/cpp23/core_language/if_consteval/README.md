# C++23 › `if consteval`

## Overview
`if consteval` (**P1938**) is a compile-time-only branch that is taken when the
enclosing function is being evaluated as part of a **manifestly constant
expression** (an *immediate function context*). It replaces the C++20 pattern
`if (std::is_constant_evaluated())`, which has a subtle bug: the condition is
itself a runtime function call, so it must be used with `if` (not `if constexpr`),
and template instantiation still sees both branches.

## Syntax
```cpp
constexpr int hash(const char* s, std::size_t n) {
    if consteval {
        // ✅ Only allowed to call other consteval / constexpr functions here.
        return fnv1a_constexpr(s, n);
    } else {
        // ✅ May use non-constexpr code (intrinsics, SIMD, syscalls, …).
        return fnv1a_runtime_simd(s, n);
    }
}
```

## `if consteval` vs `if (std::is_constant_evaluated())`
| Concern | `if consteval` (C++23) | `if (is_constant_evaluated())` (C++20) |
|---|---|---|
| Return type | Statement — not an expression | `bool` expression |
| Can gate `consteval` calls | ✅ | ❌ — both branches are still parsed / instantiated |
| Requires `<type_traits>` | ❌ | ✅ |
| Compile-time only branch | ✅ — the `else` branch is not analysed for immediate context | Both branches analysed |
| Works with `!` / `else if` | Negation only via `if !consteval` | Standard bool logic |

## Negation
```cpp
if !consteval {
    // Runtime-only path
} else {
    // Compile-time path
}
```

## Typical Use Cases
- Selecting a **portable constexpr algorithm** at compile time, and an
  **intrinsics-based** implementation at runtime (`_mm_crc32_u64`, `__builtin_*`).
- Skipping validation that would be redundant at compile time (the compiler
  already checks it).
- Avoiding `constexpr`-hostile APIs (`std::memcpy`, `reinterpret_cast`, I/O)
  during constant evaluation.

## Interaction With `consteval`
Inside a `consteval` function, `if consteval` is **always true** and mostly
redundant — but it lets you conditionally call other `consteval` helpers from
a `constexpr` function without triggering a compile error at runtime.

## Study Checklist
- [ ] Replace `if (std::is_constant_evaluated())` with `if consteval` in an existing constexpr function
- [ ] Write a `constexpr` `popcount` that uses `__builtin_popcount` at runtime and a portable loop at compile time
- [ ] Confirm you can call a `consteval` helper inside `if consteval { … }` but not outside
- [ ] Contrast with `if constexpr` — when is each correct?

## References
- [P1938 — `if consteval`](https://wg21.link/P1938)
- [cppreference — `if consteval`](https://en.cppreference.com/w/cpp/language/if#Consteval_if)
