# Advanced › Translation Units & the Compilation Model

## Overview
A **translation unit (TU)** is the basic unit of compilation: one source file
after preprocessing. Understanding TUs explains include guards, extern
declarations, and the boundary of the C++ compilation model.

## What Forms a TU
```
source.cpp
  → #include <headers>    (textual substitution)
  → after preprocessing: single translation unit
  → compiled to: object file (.o / .obj)
  → linked together: final executable / library
```

## Declarations vs Definitions
- **Declaration**: introduces a name (`extern int x;`, `void f();`)
- **Definition**: provides the implementation / allocates storage
- The **One Definition Rule (ODR)**: each non-inline entity has exactly one
  definition across all TUs.

## Internal vs External Linkage
```cpp
static int x = 0;           // internal linkage — visible only in this TU
int y = 0;                   // external linkage — visible across TUs
namespace { int z = 0; }     // anonymous namespace — internal linkage (preferred)
```

## Header Guards / `#pragma once`
Multiple `#include` of the same header within one TU must be idempotent:
```cpp
#pragma once             // or classical include guard:
#ifndef MY_HEADER_H
#define MY_HEADER_H
// ...
#endif
```

## Study Checklist
- [ ] Create two TUs sharing a global via `extern`; link and run
- [ ] Show an ODR violation by defining a function in two TUs and linking
- [ ] Compare internal linkage (`static` / anon namespace) vs external linkage
- [ ] Explain why `inline` functions can be defined in headers without ODR issues

## References
- [cppreference — Translation unit](https://en.cppreference.com/w/cpp/language/translation_phases)
