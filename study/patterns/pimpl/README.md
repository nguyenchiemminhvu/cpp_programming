# Design Pattern › PImpl — Pointer to Implementation

## Overview
PImpl (Pointer to Implementation, aka "Compilation Firewall" or "Cheshire Cat")
hides a class's private implementation in a separate heap-allocated struct,
improving ABI stability and compilation speed.

## Structure
```cpp
// widget.h — public header (minimal compile-time dependencies)
class Widget {
public:
    Widget();
    ~Widget();                   // must be defined where Impl is complete
    Widget(Widget&&) noexcept;
    Widget& operator=(Widget&&) noexcept;
    void do_something();
private:
    struct Impl;                 // forward declaration only
    std::unique_ptr<Impl> impl_;
};

// widget.cpp — implementation (heavy headers hidden here)
#include "widget.h"
#include <heavy_dependency.h>

struct Widget::Impl {
    HeavyType data;
};

Widget::Widget()  : impl_{std::make_unique<Impl>()} {}
Widget::~Widget() = default;    // needs Impl to be complete here
```

## Benefits
1. **ABI stability**: adding private members doesn't change the class layout.
2. **Faster compilation**: users of `widget.h` don't see heavy includes.
3. **Reduced header coupling** across large codebases.

## Study Checklist
- [ ] Implement a `Logger` class using PImpl
- [ ] Show that changing `Impl` members doesn't require recompiling callers
- [ ] Demonstrate why `~Widget() = default` must be in `.cpp` (incomplete type)
- [ ] Compare PImpl overhead vs direct member vs virtual interface approach

## References
- [cppreference — PImpl](https://en.cppreference.com/w/cpp/language/pimpl)
- *Effective Modern C++*, Scott Meyers — Item 22
