# C++17 › `std::filesystem`

## Overview
`<filesystem>` provides a portable, object-oriented API for path manipulation
and file system operations, replacing OS-specific POSIX/Win32 calls.

## Core Types

| Type | Purpose |
|------|---------|
| `std::filesystem::path` | Represents a file system path |
| `std::filesystem::directory_entry` | Entry returned by directory iterators |
| `std::filesystem::directory_iterator` | Non-recursive directory traversal |
| `std::filesystem::recursive_directory_iterator` | Recursive traversal |
| `std::filesystem::file_status` | File type and permissions |

## Common Operations
```cpp
namespace fs = std::filesystem;

fs::path p = "/tmp/data";
p /= "file.txt";           // path concatenation

fs::exists(p);             // bool
fs::is_regular_file(p);    // bool
fs::is_directory(p);       // bool
fs::file_size(p);          // uintmax_t
fs::last_write_time(p);    // file_time_type

fs::create_directory(p);
fs::create_directories(p); // like mkdir -p
fs::copy(src, dst);
fs::remove(p);
fs::rename(src, dst);

// Directory iteration
for (const auto& entry : fs::directory_iterator("."))
    std::cout << entry.path() << "\n";
```

## Error Handling
Operations have two overloads: one that throws `std::filesystem::filesystem_error`,
and one that takes an `std::error_code` output parameter for non-throwing use.

## Study Checklist
- [ ] Walk a directory tree recursively and list all `.cpp` files
- [ ] Implement a file copy utility with overwrite protection
- [ ] Check free disk space with `fs::space()`
- [ ] Handle errors without exceptions using the `error_code` overload
- [ ] Build and run on Linux; note any path separator differences vs Windows

## References
- [cppreference — filesystem](https://en.cppreference.com/w/cpp/filesystem)
