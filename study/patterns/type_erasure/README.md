# Design Pattern › Type Erasure

## Overview
Type erasure allows objects of different concrete types to be stored and used
through a **uniform interface**, without requiring a common base class or
templates at the storage boundary.

## Implementations

### 1. `std::function<R(Args...)>` — Simplest
```cpp
std::function<void(int)> callback = [](int x){ std::cout << x; };
callback = &free_function;   // accepts any compatible callable
```

### 2. `std::any` — For Arbitrary Values
```cpp
std::any val = 42;
val = std::string{"hello"};
std::cout << std::any_cast<std::string>(val);
```

### 3. Manual Type Erasure (concept-based / vtable)
```cpp
class Drawable {
    struct Concept {
        virtual void draw() const = 0;
        virtual ~Concept() = default;
    };
    template<typename T>
    struct Model : Concept {
        T obj;
        void draw() const override { obj.draw(); }
    };
    std::unique_ptr<Concept> self_;
public:
    template<typename T>
    Drawable(T obj) : self_{std::make_unique<Model<T>>(std::move(obj))} {}
    void draw() const { self_->draw(); }
};
```
This pattern (Sean Parent's "Polymorphism without inheritance") avoids
modifying the type being stored.

### 4. `std::variant` + `std::visit` — Closed Set
```cpp
using Shape = std::variant<Circle, Square, Triangle>;
std::visit([](auto& s){ s.draw(); }, shape);
```

## Study Checklist
- [ ] Implement a type-erased `Callable<R(Args)>` from scratch
- [ ] Compare `std::function` overhead vs raw function pointer vs template
- [ ] Build a `Drawable` container using the manual vtable pattern
- [ ] Show when `variant` + `visit` is preferable to `std::function`

## References
- Sean Parent — *Inheritance is the Base Class of Evil* (C++Now 2013)
- [cppreference — std::function](https://en.cppreference.com/w/cpp/utility/functional/function)
