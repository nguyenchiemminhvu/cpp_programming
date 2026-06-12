# C++11 › `std::error_code` & `std::system_error`

## Overview
`<system_error>` provides a portable, extensible error handling framework that
avoids exceptions for expected failures. Used extensively by `<filesystem>`,
networking libraries, and OS interfaces.

## Core Types
| Type | Role |
|------|------|
| `std::error_code` | Value-type: error value + category |
| `std::error_category` | Abstract base for error domains |
| `std::error_condition` | Portable abstract condition |
| `std::system_error` | Exception wrapping an `error_code` |

## Usage Pattern
```cpp
std::error_code ec;
std::filesystem::copy("src", "dst", ec);   // non-throwing overload
if (ec) {
    std::cerr << ec.message()
              << " [" << ec.category().name() << "]\n";
}
```

## Custom Error Category
```cpp
enum class AppError { NetworkTimeout = 1, ParseFailed };

struct AppErrorCategory : std::error_category {
    const char* name() const noexcept override { return "app"; }
    std::string message(int ev) const override {
        switch (static_cast<AppError>(ev)) {
            case AppError::NetworkTimeout: return "network timeout";
            default: return "unknown";
        }
    }
};

std::error_code make_error_code(AppError e) {
    static AppErrorCategory cat;
    return {static_cast<int>(e), cat};
}
```

## Study Checklist
- [ ] Use the non-throwing filesystem API with `error_code` output parameters
- [ ] Define a custom error category and error codes for a domain
- [ ] Convert between `system_category` codes and `generic_category` codes
- [ ] Compare `error_code` vs exception-based error handling for file I/O

## References
- [cppreference — error_code](https://en.cppreference.com/w/cpp/error/error_code)
