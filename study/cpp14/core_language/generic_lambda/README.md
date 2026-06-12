# C++14 › Generic Lambdas

## Overview
C++14 allows lambda parameters to be declared `auto`, making the compiler
generate a templated `operator()`. This is a major ergonomics upgrade over
C++11 lambdas that required concrete parameter types.

## Syntax
```cpp
// C++11 — must specify types explicitly
auto add11 = [](int a, int b) { return a + b; };

// C++14 — auto parameters → templated operator()
auto add14 = [](auto a, auto b) { return a + b; };
add14(1, 2);        // int
add14(1.5, 2.5);    // double
add14(std::string{"a"}, std::string{"b"});  // string
```

## Under the Hood
The compiler generates approximately:
```cpp
struct __Lambda {
    template<typename A, typename B>
    auto operator()(A a, B b) const { return a + b; }
};
```

## Interaction with Perfect Forwarding
```cpp
auto perfect = [](auto&& x) {
    use(std::forward<decltype(x)>(x));
};
```
`decltype(x)` inside a lambda is the canonical way to forward a generic lambda parameter.

## Study Checklist
- [ ] Write a generic `print` lambda that works for any streamable type
- [ ] Use a generic lambda as a comparator for `std::sort` on different element types
- [ ] Demonstrate the `std::forward<decltype(x)>(x)` pattern
- [ ] Compare generic lambda vs a function template: when to use which?

## References
- [cppreference — Lambda expressions (C++14)](https://en.cppreference.com/w/cpp/language/lambda)
