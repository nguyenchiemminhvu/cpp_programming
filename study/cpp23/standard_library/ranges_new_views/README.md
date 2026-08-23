# C++23 › New Range Views

## Overview
C++23 adds a large family of new **views** in `std::views` / `std::ranges`,
closing most of the "why isn't X in ranges?" complaints from C++20. Each view
is composable with the `|` pipe operator and has a lazy, non-owning
implementation.

## The New Views (papers cited in parentheses)

### Multi-range zips and products
| View | Description |
|---|---|
| `views::zip` (P2321) | Pointwise tuple of N ranges: `zip(a, b, c) → tuple(a[i], b[i], c[i])` |
| `views::zip_transform` (P2321) | `zip` + apply a callable per tuple |
| `views::adjacent<N>` (P2321) | Sliding tuples of N *consecutive* elements from one range |
| `views::adjacent_transform<N>` (P2321) | `adjacent` + callable |
| `views::cartesian_product` (P2374) | N-ary Cartesian product |

### Indexing / grouping
| View | Description |
|---|---|
| `views::enumerate` (P2164) | `(index, element)` pairs — `for (auto [i, x] : views::enumerate(v))` |
| `views::chunk<N>` (P2442) | Split into fixed-size sub-ranges |
| `views::chunk_by(pred)` (P2443) | Split every time `pred(a, b)` is false |
| `views::slide<N>` (P2442) | Sliding window of size N (overlapping) |
| `views::stride(N)` (P1899) | Every Nth element |

### Structure / string helpers
| View | Description |
|---|---|
| `views::join_with(delim)` (P2441) | `join`, but insert `delim` between sub-ranges |
| `views::split` (already in C++20 but reworked) | Standard split by delimiter |

### Generators
| View | Description |
|---|---|
| `views::repeat(v)` / `views::repeat(v, n)` (P2440) | Repeat a value infinitely or N times |
| `views::iota(a)` (already C++20) | Included for completeness — infinite counting range |

### Value-category adjustments
| View | Description |
|---|---|
| `views::as_const` (P2278) | Present a range as `const` |
| `views::as_rvalue` (P2446) | Move each element on iteration |

## Illustrative Examples

### `zip` and `enumerate`
```cpp
std::vector<std::string> names = {"a", "b", "c"};
std::vector<int>         ages  = {21, 22, 23};

for (auto [i, name_age] : std::views::enumerate(std::views::zip(names, ages)))
    std::println("{}: {} ({} years)", i, get<0>(name_age), get<1>(name_age));
```

### `chunk` — batching for I/O
```cpp
for (auto batch : items | std::views::chunk(64))
    send_batch(batch);
```

### `slide` — moving averages
```cpp
auto window3 = data | std::views::slide(3)
                    | std::views::transform([](auto w){ return std::ranges::fold_left(w, 0.0, std::plus{}) / 3.0; });
```

### `cartesian_product` — iterate a grid
```cpp
for (auto [i, j] : std::views::cartesian_product(std::views::iota(0, 4),
                                                 std::views::iota(0, 4)))
    grid[i, j] = 0;
```

### `stride` — subsample
```cpp
auto every_10th = signal | std::views::stride(10);
```

### `join_with` — string joining
```cpp
std::string csv = words | std::views::join_with(std::string_view{", "})
                        | std::ranges::to<std::string>();
```

## Range Categories
Each view preserves at most the input range category (`input` / `forward` /
`bidirectional` / `random_access`). `zip`, `adjacent`, `cartesian_product`
require **at least forward**; `chunk` / `slide` produce a range of sub-ranges.

## Availability
- GCC 14+ has most of them
- Clang 17+ (libc++ partial)
- MSVC 19.37+ (VS 2022 17.7)

## Study Checklist
- [ ] Rewrite a "parallel iterate two vectors" loop using `views::zip`
- [ ] Compute a rolling sum with `views::slide`
- [ ] Batch a sequence into fixed chunks with `views::chunk`
- [ ] Generate a 2-D coordinate grid with `views::cartesian_product`
- [ ] Convert `views::split` output to `std::vector<std::string>` via `ranges::to`
- [ ] Materialise CSV with `views::join_with` + `ranges::to`

## References
- [P2321 — `zip`](https://wg21.link/P2321)
- [P2374 — `views::cartesian_product`](https://wg21.link/P2374)
- [P2442 — `views::chunk`, `views::slide`](https://wg21.link/P2442)
- [P2443 — `views::chunk_by`](https://wg21.link/P2443)
- [P2441 — `views::join_with`](https://wg21.link/P2441)
- [P2440 — `views::repeat`](https://wg21.link/P2440)
- [P2164 — `views::enumerate`](https://wg21.link/P2164)
- [P1899 — `views::stride`](https://wg21.link/P1899)
- [P2278 — `views::as_const`](https://wg21.link/P2278)
- [P2446 — `views::as_rvalue`](https://wg21.link/P2446)
- [cppreference — Range adaptors](https://en.cppreference.com/w/cpp/ranges)
