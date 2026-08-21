# Design Pattern › Type Erasure

## Overview

**Type erasure** is a C++ technique that lets you **hide the concrete type** of an object behind a uniform interface, without requiring inheritance or virtual functions. You interact with the object through a known, stable contract — while the actual type stored underneath can be anything that satisfies that contract.

The most common real-world example is `std::function`: it can hold a free function, a lambda, a functor, or a member function pointer — all through the same `std::function<void(int)>` interface. The caller does not need to know (or care) which one it is.

### Why Not Just Use Virtual Functions?

Virtual functions solve the same problem in a classical OOP way, but they come with constraints:

- Every type that participates must **inherit** from a common base class.
- You need a **vtable** per type and a **vptr** per object (hidden overhead).
- Polymorphic objects must be accessed through **pointers or references** (no value semantics).
- The base class must be designed in advance — you cannot retrofit an existing type.

Type erasure removes all of these constraints. Any type that has the right behaviour can participate — even third-party types you do not own.

---

## The Core Mechanism: `void*` + Function Pointer

At its heart, type erasure in C++ is built from two ingredients:

1. **A raw storage buffer** — holds the object's bytes, without knowing its type.
2. **A function pointer** — remembers *how* to operate on those bytes.

Here is the minimal example from the accompanying `main.cpp`:

```cpp
constexpr uint8_t buffer[64] = {0};
using invoke_ptr_t = void(*)(void*, int);
invoke_ptr_t invoke_ptr = nullptr;
```

- `buffer` is raw memory. It does not know what type lives inside it.
- `invoke_ptr` is a function pointer that accepts `void*` (the buffer) and an `int` argument.

The type information is *erased* when the object is written into `buffer`. The function pointer *preserves* just enough knowledge to invoke it correctly later.

---

## Step 1 — Storing a Lambda

```cpp
auto free_func_lambda = [](int x) {
    std::cout << "Free function lambda called with value: " << x << std::endl;
};

// Place the lambda into raw buffer memory (no heap allocation)
new (const_cast<uint8_t*>(buffer)) decltype(free_func_lambda)(free_func_lambda);

// Create an invoker that knows the real type at construction time
invoke_ptr = [](void* storage, int arg) {
    auto& func = *static_cast<decltype(free_func_lambda)*>(storage);
    func(arg);
};

// Call later — no type knowledge needed at the call site
invoke_ptr(const_cast<uint8_t*>(buffer), 42);
```

### What is happening here?

**Placement new** — `new (address) Type(args...)` constructs an object at a *specific memory address* without allocating any heap memory. The buffer already exists; we are just putting an object into it.

**`decltype(free_func_lambda)`** — The lambda's type is unique and anonymous. `decltype` captures that type at the point where the lambda is visible, and we use it both for placement new and for the `static_cast` inside the invoker.

**The invoker lambda** — This stateless lambda (no captures) converts implicitly to a `void(*)(void*, int)` function pointer. Inside it, it casts `void*` back to the real type and calls the stored object. The type knowledge is *baked into* this function pointer at construction time and survives after the original variable goes out of scope.

The result: at the call site (`invoke_ptr(buffer, 42)`), no concrete type is referenced. The buffer is just bytes; the pointer is just a function address. **Type erasure is complete.**

---

## Step 2 — Storing a Functor

A functor is a class or struct that overloads `operator()`. It works identically to a lambda from the type erasure perspective:

```cpp
struct functor_callback {
    void operator()(int x) const {
        std::cout << "Functor callback called with value: " << x << std::endl;
    }
};

auto functor = functor_callback();
new (const_cast<uint8_t*>(buffer)) functor_callback(functor);

invoke_ptr = [](void* storage, int arg) {
    auto& func = *static_cast<functor_callback*>(storage);
    func(arg);
};

invoke_ptr(const_cast<uint8_t*>(buffer), 84);
```

The only difference is that the type (`functor_callback`) is now a named type rather than an anonymous lambda type, so the `static_cast` and placement new use it directly. The invoker function pointer still encodes the type knowledge and the buffer remains opaque.

---

## Step 3 — Storing a Class Method Call

Calling an instance method is slightly different: you need *both* the stored object *and* knowledge of which method to call.

```cpp
class callback_wrapper {
public:
    void method_callback(int x) {
        std::cout << "Method callback called with value: " << x << std::endl;
    }
};

auto wrapper = callback_wrapper();
new (const_cast<uint8_t*>(buffer)) callback_wrapper(wrapper);

invoke_ptr = [](void* storage, int arg) {
    auto& func = *static_cast<callback_wrapper*>(storage);
    func.method_callback(arg);   // method selection is encoded here
};

invoke_ptr(const_cast<uint8_t*>(buffer), 168);
```

The invoker lambda closes over both the type *and* the method name. After erasure, the caller just sees `void(*)(void*, int)`. You can swap in a different method or a different class entirely — the call site never changes.

---

## What Makes This "Type Erasure"?

At every call site, the interface is the same:

```cpp
invoke_ptr(buffer, value);
```

There is no template parameter, no base class, no `virtual` keyword. The concrete type — lambda, functor, or class instance — has been *erased* from the external interface. All that remains is:

- A blob of bytes in a buffer.
- A function pointer that knows how to work with those bytes.

This is the fundamental pattern that powers `std::function`, `std::any`, and many custom callback/signal/event systems in production C++ code.

---

## Key Concepts Summary

| Concept | Purpose |
|---|---|
| Placement new | Constructs an object at a given address without heap allocation |
| `void*` storage buffer | Holds the object's bytes in a type-agnostic way |
| Function pointer as invoker | Encodes how to call the stored object; survives type erasure |
| `decltype` / explicit cast | Recovers the concrete type *inside* the invoker at construction time |
| Stateless lambda → function pointer | A lambda with no captures converts implicitly to a plain function pointer |

---

## Important Caveats and Pitfalls

**Buffer alignment.** Raw `uint8_t` buffers are not guaranteed to be correctly aligned for arbitrary types. In production code, use `alignas(std::max_align_t)` or `std::aligned_storage` to avoid undefined behaviour on ARM and other strict-alignment architectures.

```cpp
// Prefer this in production:
alignas(std::max_align_t) uint8_t buffer[64];
```

**Buffer size.** If `sizeof(YourCallable) > sizeof(buffer)`, you silently corrupt memory. Always verify with a `static_assert`:

```cpp
static_assert(sizeof(functor_callback) <= sizeof(buffer),
    "Callable does not fit in the type-erased buffer.");
```

**Destructor must be called manually.** Objects constructed with placement new are not automatically destroyed. If the callable holds RAII resources (a `std::shared_ptr`, a file handle, a lock), you *must* call its destructor explicitly:

```cpp
static_cast<functor_callback*>(buffer)->~functor_callback();
```

**Copy/move semantics.** The raw buffer knows nothing about value semantics. If the callable has a non-trivial copy constructor, you need a separate "copier" function pointer alongside the invoker. See [`../inplace_function/README.md`](../inplace_function/README.md) for a full treatment of this.

**`const_cast` on `constexpr` buffer.** The `main.cpp` example uses `const_cast` to write through a `constexpr` buffer. This is technically undefined behaviour in Standard C++. In real code, the buffer should simply be a non-`const` variable.

---

## Real-World Applications

**`std::function`** — The standard library's type-erased callable wrapper, using the same placement-new + function-pointer technique internally (plus small-buffer optimisation and heap fallback).

**`std::any`** — Type-erased value container. Stores any copy-constructible type and retrieves it by type via `std::any_cast`.

**Signal/slot systems** — Event systems (Qt signals, Boost.Signals2, custom embedded event buses) use type erasure so that emitters do not depend on the concrete types of their receivers.

**Embedded callback tables** — In firmware and real-time systems, a table of `void(*)(void*, ...)` function pointers paired with `void*` context buffers replaces vtables, giving polymorphic dispatch with zero heap usage.

**`InplaceFunction`** — A zero-heap-allocation `std::function` alternative (see [`../inplace_function/README.md`](../inplace_function/README.md)) built entirely on this same pattern.

---

## Study Checklist

- [ ] Compile and run `main.cpp`; verify each callback prints the correct value
- [ ] Replace the `constexpr` buffer with a properly aligned non-`const` buffer
- [ ] Add explicit destructor calls for each stored object and verify with a custom destructor that prints a message
- [ ] Add a `static_assert` for buffer size safety for each stored type
- [ ] Extend the example to store a `std::function`-like wrapper using a `copy` function pointer in addition to the `invoke` pointer
- [ ] Implement a minimal `TypeErasedCallback<void(int)>` class encapsulating the buffer, invoker, destructor, and copier
- [ ] Compare with virtual dispatch: measure call overhead, object size, and code complexity

---

## References

- [cppreference — Placement new](https://en.cppreference.com/w/cpp/language/new#Placement_new)
- [cppreference — `std::function`](https://en.cppreference.com/w/cpp/utility/functional/function)
- [cppreference — `std::any`](https://en.cppreference.com/w/cpp/utility/any)
- *C++ Templates: The Complete Guide*, Vandevoorde, Josuttis & Gregor — Chapter on type erasure
- *Modern C++ Design*, Andrei Alexandrescu — Policy-based and type-erased design techniques
