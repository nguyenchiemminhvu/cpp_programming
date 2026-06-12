# C++14 › Lambda Init-Capture (`[x = expr]`)

## Overview
C++14 generalises lambda capture to allow **arbitrary expressions** as
initialisers, enabling capture-by-move and computed captures that
C++11's `[=]` / `[&]` cannot express.

## Syntax
```
[capture_name = initializer_expression]
```

## Examples
```cpp
// Capture by move — C++11 needed std::bind workaround
auto ptr = std::make_unique<Widget>();
auto f = [w = std::move(ptr)] {   // init-capture moves ptr into w
    w->use();
};

// Computed capture
int base = 10;
auto mul = [factor = base * 2](int x) { return x * factor; };  // factor = 20

// Capture a member by value (avoids capturing `this`)
struct Foo {
    int value_;
    auto get_adder() {
        return [v = value_](int x) { return x + v; };  // copies value_
    }
};
```

## C++11 Workaround (for comparison)
```cpp
// C++11 required std::bind for move-capture:
auto f = std::bind([](std::unique_ptr<Widget>& w){ w->use(); }, std::move(ptr));
```

## Study Checklist
- [ ] Move a `unique_ptr` into a lambda using init-capture
- [ ] Capture a computed value to avoid re-computing inside the lambda
- [ ] Capture a member variable by value to avoid `this` capture lifetime issues
- [ ] Show the C++11 bind workaround and explain why init-capture is superior

## References
- [cppreference — Lambda capture (init-capture)](https://en.cppreference.com/w/cpp/language/lambda#Lambda_capture)
- *Effective Modern C++*, Scott Meyers — Item 32
