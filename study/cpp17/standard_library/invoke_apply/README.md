# C++17 › `std::invoke` & `std::apply`

## Overview
`std::invoke` provides a uniform call syntax for any callable type.
`std::apply` calls a function with a tuple's elements expanded as arguments.

## `std::invoke(callable, args…)`
Handles: lambdas, function pointers, functors, member function pointers,
member data pointers — all with the same syntax:
```cpp
struct Foo { int bar(int x) { return x * 2; } };

Foo foo;
std::invoke(&Foo::bar, foo, 21);              // 42 — member function
std::invoke([](int x){ return x; }, 10);     // 10 — lambda
std::invoke(std::plus<int>{}, 3, 4);         // 7  — functor
```

## `std::apply(callable, tuple)`
```cpp
auto args = std::make_tuple(1, 2.5, "hello");
std::apply([](int a, double b, const char* c) {
    std::cout << a << " " << b << " " << c;
}, args);
```
Equivalent to `f(get<0>(t), get<1>(t), …)` using compile-time index expansion.

## Study Checklist
- [ ] Write a generic `call` wrapper using `std::invoke` that handles all callable types
- [ ] Use `std::apply` to dispatch a stored `tuple<F, Args...>` task queue entry
- [ ] Verify `std::is_invocable_v<F, Args…>` and `std::invoke_result_t<F, Args…>`
- [ ] Implement a minimal `std::apply` yourself using `make_index_sequence`

## References
- [cppreference — invoke](https://en.cppreference.com/w/cpp/utility/functional/invoke)
- [cppreference — apply](https://en.cppreference.com/w/cpp/utility/apply)
