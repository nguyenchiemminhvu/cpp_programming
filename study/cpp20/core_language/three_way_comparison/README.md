# C++20 › Three-Way Comparison (Spaceship Operator `<=>`)

## Overview
The spaceship operator `<=>` returns an ordering value and, when defaulted,
auto-generates all six comparison operators from a single declaration.

## Return Types
| Type | When to use |
|------|-------------|
| `std::strong_ordering` | All values are distinct and fully comparable (integers, `std::string`) |
| `std::weak_ordering` | Equivalence classes exist (case-insensitive strings) |
| `std::partial_ordering` | Not all pairs are comparable (floats — `NaN`) |

## Auto-Generate All Six Comparisons
```cpp
struct Point {
    int x, y;
    auto operator<=>(const Point&) const = default;
    // generates ==, !=, <, <=, >, >=
};
```

## Custom Multi-Field Comparison
```cpp
struct Version {
    int major, minor, patch;
    std::strong_ordering operator<=>(const Version& o) const {
        if (auto c = major <=> o.major; c != 0) return c;
        if (auto c = minor <=> o.minor; c != 0) return c;
        return patch <=> o.patch;
    }
    bool operator==(const Version&) const = default;
};
```

## Study Checklist
- [ ] Default `<=>` on a struct; verify all six relational operators work
- [ ] Implement a custom `<=>` for a multi-field sort key
- [ ] Show `std::partial_ordering` behaviour with `NaN` comparisons
- [ ] Replace a hand-written six-operator comparison set with defaulted spaceship

## References
- [cppreference — Three-way comparison](https://en.cppreference.com/w/cpp/language/operator_comparison#Three-way_comparison)
