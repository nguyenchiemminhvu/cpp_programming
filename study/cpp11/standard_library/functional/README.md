# C++11 › `<functional>` — `std::function` & `std::bind`

## Overview
`<functional>` provides type-erased callable wrappers, partial-application
utilities, and reference wrappers, enabling higher-order programming.

## `std::function<R(Args...)>`
A polymorphic wrapper for any callable — lambda, function pointer, functor,
member function:
```cpp
std::function<int(int, int)> add = [](int a, int b) { return a + b; };
add(2, 3);   // 5
```
**Cost**: type-erasure may involve a heap allocation for large callables.
Prefer `auto` for local lambdas; use `std::function` for callbacks stored in containers.

## `std::bind`
Creates a partial application / argument-reordering adaptor:
```cpp
auto triple = std::bind(std::multiplies<int>{}, 3, std::placeholders::_1);
triple(7);   // 21
```
In modern C++, generic lambdas are usually cleaner than `bind`.

## `std::reference_wrapper` (`std::ref` / `std::cref`)
Passes references through value-semantics APIs:
```cpp
int x = 0;
std::thread t([](int& n){ ++n; }, std::ref(x));
t.join();
```

## Study Checklist
- [ ] Store a lambda, function pointer, and functor in `std::function`; call each
- [ ] Use `std::bind` to create a unary predicate from a binary comparison function
- [ ] Measure overhead: `std::function` vs `auto` lambda in a hot loop
- [ ] Use `std::ref` to pass a variable by reference to `std::thread` or `std::bind`

## References
- [cppreference — function](https://en.cppreference.com/w/cpp/utility/functional/function)
