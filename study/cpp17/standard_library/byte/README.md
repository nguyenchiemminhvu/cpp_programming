# C++17 › `std::byte`

## Overview
`std::byte` (in `<cstddef>`) is a distinct type for raw byte manipulation.
Unlike `unsigned char` or `char`, it is **not** an integer and does not
participate in arithmetic — only bitwise operations are permitted, preventing
accidental misuse.

## Allowed Operations
```cpp
#include <cstddef>

std::byte b{0xAF};

b & std::byte{0x0F}    // AND
b | std::byte{0x10}    // OR
b ^ std::byte{0xFF}    // XOR
~b                     // NOT
b << 2                 // left shift (returns byte)
b >> 1                 // right shift

// Conversion
int val  = std::to_integer<int>(b);
std::byte b2 = static_cast<std::byte>(42);
```

## NOT Allowed (compile errors)
```cpp
b + std::byte{1};      // ERROR — no arithmetic on byte
b * 2;                 // ERROR
int x = b;             // ERROR — no implicit conversion
```

## Study Checklist
- [ ] Implement a bit-flag set using `std::byte`
- [ ] Show that `std::byte + std::byte` fails to compile
- [ ] Reinterpret an object's memory as `std::byte[]` via `reinterpret_cast`
- [ ] Compare `std::byte` vs `uint8_t` for raw memory serialisation

## References
- [cppreference — std::byte](https://en.cppreference.com/w/cpp/types/byte)
