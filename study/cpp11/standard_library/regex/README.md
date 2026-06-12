# C++11 › `<regex>` — Regular Expressions

## Overview
`<regex>` provides a standard regex library supporting ECMAScript (default),
POSIX, and other grammars via `std::regex`, `std::smatch`, and friends.

## Core Operations
```cpp
#include <regex>

std::string text = "order-2024-07-15";
std::regex  pat{R"(\d{4}-\d{2}-\d{2})"};   // raw string avoids double-escaping

// Search for first match
std::smatch m;
if (std::regex_search(text, m, pat))
    std::cout << m[0];   // "2024-07-15"

// Full-string match
bool valid = std::regex_match("hello@example.com",
                              std::regex{R"(\w+@\w+\.\w+)"});

// Replace
std::string result = std::regex_replace(text, pat, "XXXX-XX-XX");

// Iterator — all matches
for (std::sregex_iterator it{text.begin(), text.end(), pat}, end; it != end; ++it)
    std::cout << (*it)[0] << "\n";
```

## Performance Note
`std::regex` construction is expensive — compile once (e.g. `static const`) and reuse.

## Study Checklist
- [ ] Extract all integers from a string using `sregex_iterator`
- [ ] Validate an email address format with `regex_match`
- [ ] Use capture groups: `(\\d{4})-(\\d{2})-(\\d{2})` and access `m[1]`, `m[2]`, `m[3]`
- [ ] Compare `std::regex` compile time vs single `std::string::find` for simple searches

## References
- [cppreference — regex](https://en.cppreference.com/w/cpp/regex)
