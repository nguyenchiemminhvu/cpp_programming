# C++23 › Trimming Whitespace in Raw String Literals

## Overview
C++23 (**P2178** and related DRs) clarifies and standardises how the
preprocessor handles **line splices** (`\` at end of line) and trailing
whitespace when they appear *inside a raw string literal*. In C++20 the rules
had cross-compiler inconsistencies — a raw string could silently change
content depending on the compiler.

## The Rule
Inside a `R"delim(...)delim"` raw string literal:
- Line splices are **not** processed; a backslash followed by a newline is
  preserved verbatim as two characters.
- Trigraphs are ignored (long-standing rule, made explicit).
- Trailing whitespace on a line inside the raw string is preserved as-is.
- Universal character names (`\uNNNN`, `\N{NAME}`) are **not** processed
  inside raw strings — they remain literal characters.

## Example
```cpp
constexpr auto shell = R"(
    if [ -f /tmp/x ]; then \
        echo yes
    fi
)";
```
Before C++23 (with the old line-splice bug), some compilers would collapse
the `\` + newline into nothing, corrupting the shell script. C++23 guarantees
`shell` contains the backslash-newline pair unchanged.

## The Companion DR
Related core issues (CWG 2223 and friends) clarified that:
- The raw string content starts *immediately* after `R"delim(` (no leading
  whitespace consumed).
- The delimiter `delim` may be up to 16 characters, none of which may be a
  space, `(`, `)`, or backslash.
- The line-terminator inside a raw string is preserved as `\n` regardless of
  the source file's native line endings — implementations normalise.

## Practical Consequences
- Portable embedding of scripts (shell, SQL, JSON, HTML, XML) in C++ source
  is now safe across compilers.
- Multiline patch / diff-style content survives verbatim.
- No more need for hand-rolled escaping or preprocessor tricks.

## Idioms

### Embedded SQL
```cpp
constexpr auto query = R"(
    SELECT id, name
    FROM   users
    WHERE  active = 1
      AND  created_at > CURRENT_DATE - INTERVAL 30 DAY;
)";
```

### JSON snippet with escapes preserved
```cpp
constexpr auto payload = R"({"path": "C:\\Users\\me", "level": 5})";
```
The `\\` remains a single backslash escape in JSON, not a C++ escape.

### Long embedded regex
```cpp
constexpr auto rx = R"(^\d{4}-\d{2}-\d{2}$)";
```

## Study Checklist
- [ ] Embed a multi-line bash script with `\` line continuations — confirm bytes preserved
- [ ] Embed a JSON blob with `\` characters — confirm nothing is escaped by the C++ compiler
- [ ] Try a raw string containing a `\N{...}` sequence — confirm it is *not* interpreted
- [ ] Verify CR/LF handling by editing the file with CRLF endings

## References
- [P2178 — Various changes to `[cpp]` (see items 9 and 10)](https://wg21.link/P2178)
- [cppreference — raw string literals](https://en.cppreference.com/w/cpp/language/string_literal)
