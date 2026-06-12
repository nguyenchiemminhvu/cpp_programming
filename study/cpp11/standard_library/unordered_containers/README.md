# C++11 › Unordered Associative Containers

## Overview
C++11 added four hash-based containers with average **O(1)** lookup versus the
O(log n) of their ordered counterparts.

## Containers

| Container | Key type | Allows duplicate keys |
|-----------|----------|-----------------------|
| `std::unordered_map<K,V>` | unique | No |
| `std::unordered_multimap<K,V>` | non-unique | Yes |
| `std::unordered_set<K>` | unique | No |
| `std::unordered_multiset<K>` | non-unique | Yes |

## Internals
- Backed by a hash table with **open chaining** (bucket array of linked lists).
- Key performance parameters: `load_factor`, `max_load_factor`, `bucket_count`, `rehash`.
- Worst case is O(n) (all keys in one bucket); use good hash functions.

## Custom Hash
```cpp
struct MyHash {
    std::size_t operator()(const MyKey& k) const noexcept {
        return std::hash<std::string>{}(k.name) ^ (k.id << 1);
    }
};
std::unordered_map<MyKey, Value, MyHash> m;
```

## Study Checklist
- [ ] Compare `unordered_map` vs `map` lookup time on 1M elements
- [ ] Implement a custom hasher for a `struct` key
- [ ] Reserve capacity upfront (`reserve`) and measure rehash avoidance
- [ ] Iterate all elements in a bucket using `begin(n)` / `end(n)`
- [ ] Demonstrate worst-case O(n) behaviour with a deliberately bad hash

## References
- [cppreference — unordered_map](https://en.cppreference.com/w/cpp/container/unordered_map)
