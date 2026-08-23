# C++23 › `std::forward_like`

## Overview
C++23 (**P2445**) adds `std::forward_like<Model>(x)` in `<utility>` — forwards
`x` with the **value category and const-qualification** of the model type
`Model`. It is the essential companion to **deducing `this`**: when a member
function receives `this Self&& self`, you almost always want to forward a
*member* of `self` with `self`'s value category.

## Signature (essence)
```cpp
namespace std {
    template <class T, class U>
    [[nodiscard]] constexpr auto&& forward_like(U&& x) noexcept;
}
```
Given `T = Self&&` and `x` a subobject of type `M`, `forward_like<T>(x)` yields
`x` cast to the same const-ness and value category as `T`.

## Why It Exists

Without `forward_like`, forwarding a *member* through a deducing-`this`
function is verbose and error-prone:

```cpp
struct widget {
    std::string data;

    template <class Self>
    auto&& value(this Self&& self) {
        // Want: return data with the same category as self.
        using cv_ref = /* messy manual construction */;
        return static_cast<cv_ref>(self.data);
    }
};
```
With `forward_like`:
```cpp
struct widget {
    std::string data;

    template <class Self>
    auto&& value(this Self&& self) {
        return std::forward_like<Self>(self.data);
    }
};
```

## Rules — What "Like" Means

Given `T = Self&&` (i.e., an lvalue or rvalue reference):

| `Self` | Result category |
|---|---|
| `widget&` (const or not) | lvalue reference with matching const |
| `widget&&` (const or not) | rvalue reference with matching const |
| `const widget` (top-level const) | applied to result |
| `const widget&&` | `const&&` propagated |

Even if `x` is stored as a non-const member, the result respects `T`'s
const-ness — `forward_like<const T>(x)` yields a `const`-qualified reference.

## Deducing-`this` + `forward_like` Idiom
```cpp
struct string_pair {
    std::string first, second;

    template <class Self>
    auto&& first_ref(this Self&& self) {
        return std::forward_like<Self>(self.first);
    }
};

string_pair       p{"a", "b"};
const string_pair cp{"x", "y"};

p.first_ref();               // std::string&
std::move(p).first_ref();    // std::string&&
cp.first_ref();              // const std::string&
```

## Contrast With `std::forward`
- `std::forward<T>(x)` — forwards `x` with the value category of `T`,
  **assuming** `x`'s declared type matches. Requires the caller to know the
  exact form; misuse is UB.
- `std::forward_like<T>(x)` — designed for cases where `x` is a *subobject*
  whose declared type is unrelated to `T`; forwards based on `T`'s cvref-ness
  only.

## Non-Trivial Details
- The return type is a *reference*; there is no move / copy.
- Works with any value-category model, not just `Self`; occasionally useful
  for propagating the "source" reference qualification of arbitrary variables.
- `[[nodiscard]]` — using its result is the whole point.

## Study Checklist
- [ ] Implement a `value()` accessor with deducing `this` + `std::forward_like`
- [ ] Verify all four value-category cases produce the correct reference type
- [ ] Contrast a manual `static_cast<...>(self.member)` with `forward_like`
- [ ] Use `forward_like` to expose a `std::variant` alternative safely
- [ ] Read the accepted paper for corner cases involving `const&&` propagation

## References
- [P2445 — `forward_like`](https://wg21.link/P2445)
- [cppreference — `std::forward_like`](https://en.cppreference.com/w/cpp/utility/forward_like)
