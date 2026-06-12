# C++17 › `if` / `switch` with Initializer Statement

## Overview
C++17 allows an **init-statement** inside `if` and `switch` conditions,
scoping temporary variables to the conditional block and preventing name
pollution in the enclosing scope.

## Syntax
```cpp
if (init-statement; condition) { … }
switch (init-statement; expression) { … }
```

## Examples
```cpp
// Map look-up scoped to the if block
if (auto it = map.find(key); it != map.end()) {
    use(it->second);       // it only exists here
}

// Lock scoped to the if block
if (std::lock_guard lg{mutex}; data_ready) {
    process();
}

// Combine with structured bindings (C++17)
if (auto [it, inserted] = map.insert({key, val}); inserted) {
    std::cout << "New key inserted\n";
}

// switch with initializer
switch (auto result = compute(); result) {
    case 0:  handle_zero();   break;
    default: handle_other();  break;
}
```

## Study Checklist
- [ ] Refactor a `find`-then-check pattern to use `if` with init-statement
- [ ] Show that the init variable is NOT accessible outside the `if`/`else` block
- [ ] Use in a `switch` to scope a computed discriminant
- [ ] Combine `if` initializer with structured bindings for `insert` return value

## References
- [cppreference — if statement](https://en.cppreference.com/w/cpp/language/if)
