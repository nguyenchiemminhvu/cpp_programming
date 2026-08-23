# C++23 › `constexpr` Relaxations

## Overview
C++23 continues the multi-release trend of loosening what a `constexpr`
function may contain. Two headline papers:

- **P2647** — allow `static` and `thread_local` variables in `constexpr`
  functions (as long as they are not touched during constant evaluation).
- **P2242** — allow non-literal variables, `goto`, and labels in `constexpr`
  functions (again, only checked when the function *is* constant-evaluated).

The philosophy: **defer diagnostics to the point of evaluation**. A `constexpr`
function may now contain runtime-only code as long as no *constant* evaluation
executes it.

## What's Now Legal

### Static / thread_local variables
```cpp
constexpr int cached_lookup(int i) {
    static int table[256] = { /* filled at runtime */ };
    if consteval { return i * i; }        // ✅ never touches `table` at compile time
    return table[i];
}
```

### Non-literal variables and `goto` / labels
```cpp
constexpr int example(int n) {
    if consteval {
        return n * n;                     // ✅ compile-time path
    }
    NonLiteral obj;                       // ✅ legal in C++23, evaluated only at runtime
    int i = 0;
loop:
    if (i++ < n) goto loop;               // ✅ allowed at runtime
    return i;
}
```

### Trivial default initialisation
`constexpr` variables may now be default-initialised without an explicit
initialiser in some cases (previous rules required all subobjects to be
initialised).

## Rules Still In Force
- If a call reaches non-literal code during **constant evaluation**, the
  program is ill-formed.
- `static` / `thread_local` values may not be *modified* or *read* during
  constant evaluation.
- `try` / `throw` in `constexpr` is C++20; `throw` at compile time is still
  only allowed inside a `consteval` context that never actually reaches it.

## Companion Feature: `if consteval`
Together with `if consteval` (see sibling folder), these relaxations enable
**dual-implementation** functions that pick a portable branch at compile time
and a fast runtime branch (SIMD, intrinsics, syscalls) at runtime — all under
one `constexpr` declaration.

## Study Checklist
- [ ] Write a `constexpr` function with a `static` LUT used only at runtime
- [ ] Add a `goto` loop inside a `constexpr` function; call it at compile time and confirm the compiler still accepts *the compile-time path only*
- [ ] Attempt to modify a `static` variable during constant evaluation — verify the diagnostic
- [ ] Pair with `if consteval` to build a portable-plus-SIMD `constexpr` hash
- [ ] Compare permitted C++20 vs C++23 constexpr surface using a small test file

## References
- [P2242 — Non-literal variables, goto, labels in `constexpr`](https://wg21.link/P2242)
- [P2647 — Permitting `static constexpr` variables in `constexpr` functions](https://wg21.link/P2647)
- [cppreference — constexpr function](https://en.cppreference.com/w/cpp/language/constexpr)
