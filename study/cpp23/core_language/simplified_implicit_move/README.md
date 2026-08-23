# C++23 › Simplified Implicit Move

## Overview
C++23 (**P2266**) reworks the implicit-move rules for `return`, `co_return`,
`co_yield`, and `throw` so that named parameters and by-value locals are
consistently treated as **rvalues** on their final use — even when the return
type is a reference. This closes long-standing surprises where NRVO / implicit
move failed silently.

## The Old Rule (C++11 – C++20)
Implicit move applied only when:
- The operand is an id-expression naming an **automatic** variable, and
- The variable's **declared type** matches the function's return type modulo
  cv-qualifiers.

Consequences:
```cpp
struct base {};
struct derived : base {};

base f() {
    derived d;
    return d;                 // ❌ pre-C++23: type mismatch → copies, no move
}

std::string g(std::string s) {
    return s;                 // ✅ move — declared type matches return type
}

std::string h(std::string&& s) {
    return s;                 // ❌ pre-C++23: `s` is an lvalue reference param → copy
}
```

## The C++23 Rule
Implicit move now applies to *any* automatic variable or function parameter
whose id-expression is used in a return / co_return / co_yield / throw
operand — regardless of the declared-type match. The compiler will still fall
back to a copy if move overload resolution fails.

```cpp
base f() {
    derived d;
    return d;                 // ✅ C++23: derived → base by move
}

std::string h(std::string&& s) {
    return s;                 // ✅ C++23: move
}
```

## Impact
- More consistent codegen with fewer surprising copies.
- Removes many hand-written `return std::move(x);` — which is now redundant
  and often *inhibits* NRVO. C++23 idiom: **just `return x;`**.
- Legacy code that relied on the strict rule (rare) may see behavioural
  changes if a move constructor has different observable effects than a copy.

## Best Practices
- Do **not** write `return std::move(local);` — write `return local;` and let
  the compiler apply NRVO first, implicit move second.
- Do **not** write `return std::move(param);` in most cases — same reason.
  Exceptions: forwarding parameters (`return std::forward<T>(param);`), or
  when returning a **member** rather than the parameter itself.

## Study Checklist
- [ ] Return a derived type as a base type; observe move vs copy on C++20 and C++23 compilers
- [ ] Rewrite `return std::move(local);` as `return local;` and check assembly for equivalence
- [ ] Test with `std::string&&` and `T&&` parameters — confirm C++23 moves, C++20 copies
- [ ] Verify `throw x;` also moves in C++23
- [ ] Discuss when `std::move` is still necessary at return (member access, forwarding)

## References
- [P2266 — Simpler implicit move](https://wg21.link/P2266)
- [cppreference — return statement](https://en.cppreference.com/w/cpp/language/return)
