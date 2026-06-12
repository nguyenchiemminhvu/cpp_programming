# C++14 › `std::quoted`

## Overview
`std::quoted` is an I/O manipulator in `<iomanip>` that wraps a string in
quotes on output and correctly parses quoted strings (including embedded
spaces and escaped characters) on input.

## Syntax
```cpp
#include <iomanip>
#include <sstream>

// Output
std::string s = "hello world";
std::cout << std::quoted(s);             // "hello world"

// Round-trip: write then read back
std::ostringstream oss;
oss << std::quoted("path with spaces");  // "path with spaces"

std::istringstream iss{oss.str()};
std::string result;
iss >> std::quoted(result);              // result = "path with spaces"
```

## Custom Delimiter and Escape
```cpp
// Using single-quote delimiter
std::cout << std::quoted(s, '\'', '\\');   // 'hello world'
```

## Primary Use Cases
1. **Round-tripping strings** through stream I/O when spaces are present.
2. **Config file / log output** that requires proper string quoting.
3. **CSV serialisation** where field values may contain the delimiter.

## Study Checklist
- [ ] Write and read back a `vector<string>` using `quoted` for proper space handling
- [ ] Show how `>>` without `quoted` stops at whitespace
- [ ] Implement a simple CSV serialiser using `quoted`
- [ ] Use `quoted` with a `stringstream` to tokenise a command line

## References
- [cppreference — quoted](https://en.cppreference.com/w/cpp/io/manip/quoted)
