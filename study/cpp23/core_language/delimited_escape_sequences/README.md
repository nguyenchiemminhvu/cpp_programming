# C++23 › Delimited Escape Sequences

## Overview
C++23 (**P2290**) introduces **braced** numeric escape sequences that end at a
closing `}`. The classic `\xNN`, `\NNN` (octal), and `\uNNNN` forms remain
supported, but they have long-standing ambiguity problems that the new syntax
eliminates.

## The Three Forms

| New syntax | Meaning | Base |
|---|---|---|
| `\x{...}` | Hexadecimal escape | 16 |
| `\o{...}` | Octal escape | 8 |
| `\u{...}` | Universal character (code point) | 16 |

Examples:
```cpp
constexpr char32_t pi   = U'\u{03C0}';        // GREEK SMALL LETTER PI
constexpr char     tab  = '\x{09}';
constexpr char     bell = '\o{7}';
const char*        s    = "\x{1F}color\x{1F}";  // no ambiguity with following digits
```

## Why the Braces Matter
### `\x` was greedy
Legacy `\x` swallowed **all** following hex digits — a single-byte escape next
to `"cafe"` becomes chaos:
```cpp
const char* bad = "\xcafe";      // ❌ pre-C++23: one huge escape; error or UB
const char* ok  = "\x{c}afe";    // ✅ C++23: escape is 0x0C then "afe"
```
### Octal `\NNN` had a fixed max-3-digit rule
```cpp
"\7A"                            // '\007' + 'A' — hard to spot the boundary
"\o{7}A"                         // clear
```
### `\uNNNN` required exactly 4 or 8 digits
```cpp
"\u1F"                           // ❌ pre-C++23: not enough digits
"\u{1F}"                         // ✅ C++23: fine
```

## Rules
- The braced form must contain **at least one digit** in the appropriate base.
- No embedded whitespace or comments inside the braces.
- The value must be representable in the target character type / encoding.
- Universal character escapes (`\u{...}`) may specify **any** valid Unicode
  code point — no more 4- or 8-digit padding.

## Companion Feature
`\N{NAME}` — named Unicode escape — is documented in the sibling folder
`named_universal_char_escapes`. Together they give you three orthogonal ways
to spell any character: symbolic (`\N{…}`), hex (`\x{…}` / `\u{…}`), and octal
(`\o{…}`).

## Study Checklist
- [ ] Reproduce the classic `\xcafe` ambiguity, then fix with `\x{c}afe`
- [ ] Write a `char32_t` string using `\u{1F600}` (grinning face 😀)
- [ ] Use `\o{}` in a byte-array test to make bit patterns explicit
- [ ] Combine `\x{…}` with `\N{…}` in the same literal
- [ ] Confirm the compiler rejects empty braces `\x{}`

## References
- [P2290 — Delimited escape sequences](https://wg21.link/P2290)
- [cppreference — escape sequences](https://en.cppreference.com/w/cpp/language/escape)
