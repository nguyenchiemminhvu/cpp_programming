# Design Pattern › RAII — Resource Acquisition Is Initialization

## Overview
RAII ties a resource's lifetime to a C++ object's scope. The resource is
acquired in the constructor and released in the destructor, guaranteeing
release even when exceptions are thrown.

## Classic Implementation
```cpp
class FileHandle {
public:
    explicit FileHandle(const char* path, const char* mode)
        : file_{std::fopen(path, mode)} {
        if (!file_) throw std::runtime_error{"cannot open file"};
    }
    ~FileHandle() { if (file_) std::fclose(file_); }

    // Non-copyable (unique ownership)
    FileHandle(const FileHandle&)            = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    // Movable
    FileHandle(FileHandle&& o) noexcept
        : file_{std::exchange(o.file_, nullptr)} {}
    FileHandle& operator=(FileHandle&&) noexcept = default;

    FILE* get() const noexcept { return file_; }
private:
    FILE* file_;
};
```

## Standard Library RAII Types
| Resource | RAII Wrapper |
|----------|-------------|
| Heap memory | `std::unique_ptr`, `std::shared_ptr` |
| Mutex | `std::lock_guard`, `std::unique_lock`, `std::scoped_lock` |
| File | `std::fstream` |
| Thread | `std::jthread` (C++20) |

## Study Checklist
- [ ] Implement an RAII socket handle with proper move semantics
- [ ] Show that RAII guarantees release even on exception paths
- [ ] Build a `ScopeGuard` that executes a lambda on scope exit
- [ ] Explain why `std::unique_ptr` is the canonical RAII type for heap memory

## References
- [cppreference — RAII](https://en.cppreference.com/w/cpp/language/raii)
