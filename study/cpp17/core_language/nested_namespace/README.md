# C++17 › Nested Namespace Definition

## Overview
C++17 allows nested namespace definitions in a single declaration using
the `::` separator, eliminating deeply-nested curly braces.

## Before C++17
```cpp
namespace A {
    namespace B {
        namespace C {
            void func();
        }
    }
}
```

## C++17 Syntax
```cpp
namespace A::B::C {
    void func();
}
```

## C++20 Extension — Inline Nested
```cpp
namespace A::B::inline C {   // C is inline within B
    void func();
}
```

## Common Library Patterns
```cpp
// Top-level public API + detail namespace
namespace mylib::detail {
    void internal_helper();
}
namespace mylib {
    void process() { detail::internal_helper(); }
}
```

## Study Checklist
- [ ] Refactor a 3-level nested namespace to C++17 `::` syntax
- [ ] Show that old multi-brace and new `::` form are fully equivalent
- [ ] Define `project::net::http` namespace and add functions inside it
- [ ] Explore `inline namespace` combined with nested syntax (C++20)

## References
- [cppreference — Namespace](https://en.cppreference.com/w/cpp/language/namespace)
