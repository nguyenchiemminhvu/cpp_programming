# C++23 › `std::string::contains`

## Overview
C++23 (**P1679**) adds `contains` to `std::basic_string` and
`std::basic_string_view`, joining `starts_with` / `ends_with` (added in C++20).
It returns a plain `bool` and covers the three overloads you would expect:
substring, single character, and C-string.

## Signatures
```cpp
constexpr bool contains(std::basic_string_view<CharT, Traits> sv)  const noexcept;
constexpr bool contains(CharT c)                                    const noexcept;
constexpr bool contains(const CharT* s)                             const;
```
Both `std::string` and `std::string_view` expose them.

## Basic Usage
```cpp
std::string s = "the quick brown fox";

s.contains("quick");   // true
s.contains('q');       // true
s.contains("Q");       // false — case-sensitive
```

## Comparison With Older Idioms
| Pre-C++23 | C++23 |
|---|---|
| `s.find("x") != std::string::npos` | `s.contains("x")` |
| `std::string_view{s}.find(c) != …` | `s.contains(c)` |
| `boost::algorithm::contains(s, x)` | Standard, no dependency |

## `constexpr` and `noexcept`
- All three overloads are `constexpr` — you can use them in `constexpr` string
  processing.
- The two `string_view` and `char` overloads are `noexcept`; the `const CharT*`
  overload is not (it may access memory through an untrusted pointer).

## Character-Type Coverage
Available for every `basic_string` / `basic_string_view` instantiation:
- `std::string` / `std::string_view` (`char`)
- `std::wstring` / `std::wstring_view` (`wchar_t`)
- `std::u8string` / `std::u8string_view` (`char8_t`)
- `std::u16string` / `std::u16string_view` (`char16_t`)
- `std::u32string` / `std::u32string_view` (`char32_t`)

## Design Notes
- Runtime complexity is the same as `find` (naive/linear in the standard's
  wording; implementations often use SBOM / Boyer-Moore for long needles).
- No overload takes an *iterator range* — `contains` is intentionally a
  simple, obvious API.

## Study Checklist
- [ ] Replace `.find(x) != npos` idioms in a codebase and compare readability
- [ ] Use `contains` inside a `constexpr` function
- [ ] Benchmark against `std::search` / `std::boyer_moore_searcher` for long strings
- [ ] Confirm the `char*` overload is not `noexcept` — why?
- [ ] Test with `std::u8string_view` for UTF-8 substring checks

## References
- [P1679 — `contains` function for basic_string / string_view](https://wg21.link/P1679)
- [cppreference — `std::string::contains`](https://en.cppreference.com/w/cpp/string/basic_string/contains)
