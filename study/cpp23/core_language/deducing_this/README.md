# C++23 › Deducing `this` (Explicit Object Parameter)

## Overview
Deducing `this` (**P0847**) lets you write the object parameter of a member
function like any other parameter — as an *explicit object parameter*. This
enables a single member function to deduce the object's **type**, **const-ness**,
and **value category** without writing four overloads (`&`, `const&`, `&&`,
`const&&`), and unlocks CRTP-free static polymorphism.

## Syntax
```cpp
struct widget {
    template <typename Self>
    auto&& value(this Self&& self) {
        return std::forward<Self>(self).data;
    }
    std::string data;
};

widget       w{"x"};
const widget cw{"y"};

w.value();               // Self = widget&        → std::string&
cw.value();              // Self = const widget&  → const std::string&
std::move(w).value();    // Self = widget&&       → std::string&&
```
The keyword `this` inside the parameter list marks the *explicit object
parameter*; it is the very first parameter and replaces the implicit `this`
pointer.

## What It Replaces
| Traditional C++20 | C++23 with deducing `this` |
|---|---|
| Four overloads: `T&`, `T const&`, `T&&`, `T const&&` | One templated member |
| CRTP: `template<class D> struct Base { … static_cast<D&>(*this) … }` | Direct `Self&&` deduction |
| `enable_if` / `requires` on `this` type | Constrain `Self` directly |

## CRTP-Free Static Polymorphism
```cpp
struct add_postfix_increment {
    template <typename Self>
    auto operator++(this Self&& self, int) {
        auto tmp = self;
        ++self;
        return tmp;
    }
};

struct counter : add_postfix_increment {
    counter& operator++() { ++n_; return *this; }
    int n_ = 0;
};
```
`counter c; c++;` works — no CRTP boilerplate.

## Recursive Lambdas
```cpp
auto fact = [](this auto self, int n) -> int {
    return n <= 1 ? 1 : n * self(n - 1);
};
```

## Pitfalls
- `Self` may be **the derived type** when called via inheritance — take care
  when accessing base members.
- No `virtual` deducing-`this` members (immediate object parameter forbids it).
- Combine with `std::forward_like` (C++23) to forward *members* with the right
  value category:
  ```cpp
  template <typename Self>
  auto&& get(this Self&& self) {
      return std::forward_like<Self>(self.data);
  }
  ```

## Study Checklist
- [ ] Replace four ref-qualified overloads with a single deducing-`this` template
- [ ] Rewrite a CRTP mixin (e.g. `equality_comparable`) using deducing `this`
- [ ] Write a recursive lambda using `this auto self`
- [ ] Combine deducing `this` with `std::forward_like` to correctly forward a member
- [ ] Verify the compiler emits one instantiation per (type, cvref) actually used

## References
- [P0847 — Deducing `this`](https://wg21.link/P0847)
- [cppreference — explicit object parameter](https://en.cppreference.com/w/cpp/language/member_functions#Explicit_object_parameter)
