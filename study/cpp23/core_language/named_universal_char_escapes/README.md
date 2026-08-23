# C++23 › Named Universal Character Escapes

## Overview
C++23 (**P2071**) adds `\N{NAME}` escapes that refer to Unicode characters by
their **official Unicode name** rather than a numeric code point. This makes
source code with symbolic characters vastly more readable and
grep-friendly, especially in tests, math, and localisation code.

## Syntax
```cpp
constexpr auto pi   = U"\N{GREEK SMALL LETTER PI}";     // U'π'
constexpr auto beta = U'\N{GREEK SMALL LETTER BETA}';   // U'β'
const auto snowman  = "\N{SNOWMAN}";                    // "☃"
```
The name inside the braces is matched **case-insensitively** and must be an
official Unicode character name (as of the Unicode version supported by the
compiler).

## Works In All String / Character Kinds
- Narrow: `"\N{...}"`
- `char8_t`: `u8"\N{...}"`
- `char16_t`: `u"\N{...}"`
- `char32_t`: `U"\N{...}"`
- Wide: `L"\N{...}"`
- Character literals: `'\N{...}'` (must fit the encoding)

## Why It Matters
Before C++23:
```cpp
const char* pi = "\u03C0";       // 🤔 what character is this again?
```
After C++23:
```cpp
const char* pi = "\N{GREEK SMALL LETTER PI}";   // 😊 obvious
```

## Errors
- Unknown name → compile error (does *not* silently insert `\uFFFD`).
- Name maps to a code point that doesn't fit the target encoding (e.g.
  `\N{SNOWMAN}` in a narrow `char` literal when the execution encoding is
  ASCII) → compile error.

## Interaction With Other C++23 Escapes
`\N{...}` is the "third form" alongside the two other C++23 escape improvements:
- Delimited numeric escapes: `\u{03C0}`, `\x{2603}`, `\o{377}` (see sibling
  folder `delimited_escape_sequences`).
- Named escapes: `\N{GREEK SMALL LETTER PI}` (this feature).

## Study Checklist
- [ ] Rewrite a Unicode-heavy literal using `\N{...}` and confirm readability
- [ ] Try `u8"\N{SNOWMAN}"` — verify UTF-8 encoding matches expected bytes
- [ ] Attempt an unknown name — observe the compile error
- [ ] Combine `\N{...}` with `std::print("{}\n", ...)` in a small demo

## References
- [P2071 — Named universal character escapes](https://wg21.link/P2071)
- [cppreference — escape sequences](https://en.cppreference.com/w/cpp/language/escape)
- [Unicode Character Database](https://unicode.org/ucd/) — for canonical names
