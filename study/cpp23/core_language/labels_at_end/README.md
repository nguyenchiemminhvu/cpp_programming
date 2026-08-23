# C++23 › Labels at the End of a Compound Statement

## Overview
C++23 (**P2324**) fixes a decades-old inconsistency: in C, a label may be the
**last** thing in a block (e.g. an `end:` label with nothing after it), but in
C++ every label had to be followed by a statement — forcing programmers to add
an empty statement `;`. C++23 aligns C++ with C by allowing labels at the end
of a compound statement.

## Syntax
```cpp
void do_work() {
    for (int i = 0; i < 10; ++i) {
        if (should_stop(i)) goto cleanup;
        step(i);
    cleanup:                       // ✅ C++23: no trailing `;` needed
    }
}
```

## Before C++23
```cpp
void do_work_cpp20() {
    for (int i = 0; i < 10; ++i) {
        if (should_stop(i)) goto cleanup;
        step(i);
    cleanup: ;                     // ← empty statement required
    }
}
```

## Applies To All Label Kinds
- `identifier:`
- `case value:`
- `default:`

For example, a trailing `default:` in a `switch` no longer needs `break;`:
```cpp
switch (n) {
    case 1: handle_one(); break;
    default:                       // ✅ C++23: legal at end of block
}
```

## Practical Value
- Reduces boilerplate in C-style resource-cleanup code compiled as C++
- Makes it easier to port C headers and code (fewer diagnostic surprises)
- Removes a small syntactic wart in machine-generated code

## Study Checklist
- [ ] Write a `goto cleanup;` pattern with a trailing `cleanup:` label
- [ ] Confirm your C++20 compiler rejects it and your C++23 compiler accepts it
- [ ] Test the same behaviour with `default:` in a `switch`
- [ ] Grep a legacy C-in-C++ codebase for `label: ;` — candidates for cleanup

## References
- [P2324 — Labels at the end of compound statements](https://wg21.link/P2324)
- [cppreference — labeled statements](https://en.cppreference.com/w/cpp/language/statements#Labels)
