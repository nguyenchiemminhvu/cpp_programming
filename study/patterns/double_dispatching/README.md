# Design Pattern › Double Dispatching

## Overview
Double Dispatching is a technique that lets you pick the right function to call
based on the **runtime types of two objects**, not just one. Normal virtual
functions (single dispatch) only look at the type of the object you call the
method on. Double dispatch adds a second step so both objects get a say in
which function runs.

## The Problem
In C++, when you call a virtual function on a base pointer, the compiler picks
the right override based on the actual object type. But it only checks **one**
object — the one before the dot.

```cpp
game_object* a = new player();
game_object* b = new enemy();
a->collide_with(*b);  // virtual dispatch resolves 'a' to player,
                       // but 'b' is still seen as game_object&
```

We want different behaviour for every **pair** of types (player+player,
player+enemy, enemy+enemy, etc.), but a single virtual call can't do that.

## How It Works
The trick uses **two virtual calls** instead of one:

1. **First dispatch** — call `collide_with(game_object&)` on the first object.
   Virtual dispatch resolves the first object's real type.
2. **Second dispatch** — inside that override, call back on the second object
   with a method that names the first object's type explicitly (e.g.
   `other.collide_with_player(*this)`). This second virtual call resolves the
   second object's real type.

After two hops, both types are known and the correct function runs.

## Structure
```
game_object  (base)
├── collide_with(game_object&)          ← first dispatch (pure virtual)
├── collide_with_player(player&)        ← second dispatch helpers
└── collide_with_enemy(enemy&)

player : game_object
├── collide_with(game_object& other)    → other.collide_with_player(*this)
├── collide_with_player(player&)        → "Player ↔ Player"
└── collide_with_enemy(enemy&)          → "Player ↔ Enemy"

enemy : game_object
├── collide_with(game_object& other)    → other.collide_with_enemy(*this)
├── collide_with_player(player&)        → "Enemy ↔ Player"
└── collide_with_enemy(enemy&)          → "Enemy ↔ Enemy"
```

## Example
```cpp
class game_object {
public:
    virtual ~game_object() = default;
    virtual void collide_with(game_object& other) = 0;        // 1st dispatch
    virtual void collide_with_player(player& p) = 0;          // 2nd dispatch
    virtual void collide_with_enemy(enemy& e) = 0;            // 2nd dispatch
};

class player : public game_object {
public:
    void collide_with(game_object& other) override {
        other.collide_with_player(*this);   // "I am a player, now you decide"
    }
    void collide_with_player(player&) override {
        std::cout << "Player collides with Player\n";
    }
    void collide_with_enemy(enemy&) override {
        std::cout << "Player collides with Enemy\n";
    }
};

class enemy : public game_object {
public:
    void collide_with(game_object& other) override {
        other.collide_with_enemy(*this);    // "I am an enemy, now you decide"
    }
    void collide_with_player(player&) override {
        std::cout << "Enemy collides with Player\n";
    }
    void collide_with_enemy(enemy&) override {
        std::cout << "Enemy collides with Enemy\n";
    }
};

// Usage
player p;
enemy e;
p.collide_with(e);   // Player collides with Enemy
e.collide_with(p);   // Enemy collides with Player
```

## Step-by-Step Walkthrough
```
p.collide_with(e)
│
├─ 1st dispatch: p is a player → player::collide_with(game_object& other)
│     calls other.collide_with_player(*this)
│
└─ 2nd dispatch: other is actually an enemy → enemy::collide_with_player(player&)
      prints "Enemy collides with Player"
```

## Pros and Cons

| Pros | Cons |
|------|------|
| Works with base pointers / references | Adding a new type means updating **every** existing class |
| No `dynamic_cast` or type checks needed | Number of methods grows as N² (N = number of types) |
| Fully resolved at each virtual call | Can get verbose for many types |

## When to Use
- **Collision systems** in games — different reactions for each pair of objects.
- **AST visitors** — an operation depends on both the visitor type and the node type.
- Any time behaviour depends on the **combination** of two polymorphic types.

## Alternatives
- **Visitor Pattern** — a more structured form of double dispatch.
- **`std::variant` + `std::visit`** — modern C++17 approach, no inheritance needed.
- **Type-id map** — use `typeid` pairs as keys into a function table.

## Study Checklist
- [ ] Trace the two virtual calls on paper for `p.collide_with(e)`
- [ ] Add a third type (e.g. `obstacle`) and see how many methods you need to add
- [ ] Rewrite the example using `std::variant` + `std::visit` and compare
- [ ] Relate double dispatch to the Visitor pattern

## References
- *Design Patterns*, Gamma et al. — Visitor pattern chapter
- [Wikipedia — Double dispatch](https://en.wikipedia.org/wiki/Double_dispatch)
- [cppreference — std::visit](https://en.cppreference.com/w/cpp/utility/variant/visit)
