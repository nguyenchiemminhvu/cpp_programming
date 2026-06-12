# C++11 › Delegating Constructors

## Overview
A delegating constructor calls another constructor of the **same class** in
its member-initializer list, eliminating duplicated initialization logic.

## Syntax
```cpp
class Config {
public:
    Config() : Config(8080) {}                        // delegates
    Config(int port) : Config(port, "localhost") {}   // delegates
    Config(int port, std::string host)                // target
        : port_{port}, host_{std::move(host)} {}
private:
    int         port_;
    std::string host_;
};
```

## Rules
- A delegating constructor **cannot** have any other initializers.
- If the target constructor throws, the delegating constructor body is skipped.
- Delegation can chain; cycles are **ill-formed**.
- The object is fully constructed once any constructor in the chain completes.

## Before C++11 (init helper workaround)
```cpp
class Widget {
    void init(int a, int b) { a_ = a; b_ = b; }   // manual helper
public:
    Widget()       { init(0, 0); }
    Widget(int a)  { init(a, 0); }
    Widget(int a, int b) { init(a, b); }
};
```

## Study Checklist
- [ ] Refactor a class with 3+ constructors sharing init logic using delegation
- [ ] Show the compiler error for a self-delegating constructor
- [ ] Demonstrate that object lifetime starts upon successful delegation
- [ ] Combine delegation with `= default` and `= delete`

## References
- [cppreference — Delegating constructors](https://en.cppreference.com/w/cpp/language/constructor#Delegating_constructors)
