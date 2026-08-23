# C++23 › UTF-8 as Portable Source File Encoding

## Overview
C++23 (**P2295**) makes **UTF-8** a required, portable source-file encoding:
a conforming compiler must accept a UTF-8-encoded source file (with or without
BOM) and interpret the basic and extended character sets accordingly. This
finally standardises what nearly all modern toolchains already do.

## What Changes

### Before C++23
The source character set was **implementation-defined**. Two compilers could
disagree on the meaning of the exact same bytes in a string literal. Portable
projects needed to:
- Force `/utf-8` on MSVC, `-finput-charset=UTF-8` on GCC / Clang.
- Restrict identifiers to ASCII.
- Use `\uNNNN` escapes for non-ASCII characters.

### With C++23
- UTF-8 source is a required, portable option — no compiler flags needed for
  literal `char*` / `u8"…"` / `u"…"` / `U"…"` / `L"…"` string content.
- BOM is optional and, when present, silently accepted.
- Identifiers may contain UAX #31 letters (already allowed in C++11 via
  universal character names, now directly readable in source).

## What Doesn't Change
- The **execution character set** for narrow `char` literals is still
  implementation-defined. If you need portable UTF-8 at runtime, prefer
  `u8"…"` and `char8_t*` (C++20).
- Encoding of comments and identifiers still requires the compiler to
  understand the encoding — UTF-8 is now guaranteed to be understood; other
  encodings remain implementation-defined.

## Practical Consequences

### Identifiers with non-ASCII characters
```cpp
double π = 3.141592653589793;      // ✅ portable in C++23
double calcΔ(double a, double b) { return b - a; }
```

### Direct UTF-8 in narrow literals
```cpp
const char* greeting = "Hello, 世界!";     // ✅ UTF-8 bytes preserved by the compiler
```
Whether the runtime treats them as UTF-8 is a **separate** question — pair
with `u8"…"` and `char8_t*` for guaranteed UTF-8.

### File / BOM handling
The compiler must accept the sequence `EF BB BF` at the start of the file as
a UTF-8 BOM and discard it.

## Interaction With Other C++23 Features
- Combined with `\N{...}` (named escapes) and `\u{...}` (delimited escapes),
  you can now mix direct-UTF-8 characters and named escapes in the same
  literal, whichever is clearest per line.

## Migration Tips
- Save your source files as **UTF-8 without BOM** for maximum tool compatibility.
- On MSVC, drop `/utf-8` only when your **entire** dependency chain is C++23.
- Watch for CI that lints file encodings — update to accept UTF-8-without-BOM.

## Study Checklist
- [ ] Save a `.cpp` file as UTF-8 (no BOM), include a non-ASCII string literal, and compile with all three toolchains
- [ ] Repeat with BOM — confirm identical behaviour
- [ ] Compare `char*` vs `char8_t*` runtime bytes
- [ ] Use a non-ASCII identifier and confirm compilation
- [ ] Investigate what your compiler does with a `Latin-1` source file (should be diagnostic in C++23)

## References
- [P2295 — Support for UTF-8 as a portable source file encoding](https://wg21.link/P2295)
- [cppreference — character sets and encodings](https://en.cppreference.com/w/cpp/language/charset)
