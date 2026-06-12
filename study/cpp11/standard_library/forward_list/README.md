# C++11 › `std::forward_list<T>` — Singly-Linked List

## Overview
`std::forward_list<T>` is a singly-linked list optimised for minimal per-element
overhead (one pointer vs two for `std::list`). It supports O(1) insertion and
erasure at the front and after a known position.

## vs `std::list`
| | `std::forward_list` | `std::list` |
|-|---------------------|-------------|
| Links per node | 1 (forward only) | 2 (prev + next) |
| `size()` | No (use `std::distance`) | O(1) |
| Reverse iteration | No | Yes |
| Memory per node | Smaller | Larger |

## Key API Differences
```cpp
std::forward_list<int> fl{1, 2, 3};

fl.push_front(0);                        // add to front — O(1)
fl.insert_after(fl.begin(), 99);         // insert AFTER iterator
fl.erase_after(fl.begin());              // erase element AFTER iterator
fl.before_begin();                       // sentinel for insert at front

fl.splice_after(fl.before_begin(), other, other.begin()); // O(1) node transfer
```

## Study Checklist
- [ ] Build a sorted `forward_list` using `insert_after` with a linear search
- [ ] Implement a simple stack using `forward_list` (push_front / pop_front)
- [ ] Compare memory use per node with `std::list<int>` using `sizeof`
- [ ] Demonstrate `splice_after` for O(1) node transfer between two lists

## References
- [cppreference — forward_list](https://en.cppreference.com/w/cpp/container/forward_list)
