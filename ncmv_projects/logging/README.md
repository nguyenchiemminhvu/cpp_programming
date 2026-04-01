# logging

Header-only, thread-safe C++ logging library — single file, C++14 compatible, no external dependencies.

---

## Files

```
logging.hpp          ← entire library (include this)
test/
  test_logging.cpp   ← usage examples and scenarios
```

---

## Quick start

```cpp
#include "logging.hpp"

ncmv::logger lg;

lg.log_info("Server started on port ", 8080);
lg.log_debug("Request id=", 42, " payload=", payload);
lg.log_warning("Disk usage at ", 87, "%");
lg.log_error("Connection refused on port ", 8080);

// or with an explicit level
lg.log(ncmv::log_level::info, "hello ", "world");
```

Default output format:

```
[0ms][INFO ] Server started on port 8080
[1ms][DEBUG] Request id=42 payload=...
[2ms][WARN ] Disk usage at 87%
[3ms][ERROR] Connection refused on port 8080
```

---

## Configuration

```cpp
// Minimum log level (filters out everything below)
lg.set_min_level(ncmv::log_level::warning);

// Direct to a different stream
ncmv::logger lg2(std::cerr);

// Inject a custom formatter
lg.set_formatter(std::make_shared<my_json_formatter>());

// Inject a custom sink
lg.set_sink(std::make_shared<my_file_sink>("app.log"));

// Full dependency injection
ncmv::logger lg3(
    std::make_shared<my_sink>(),
    std::make_shared<my_formatter>(),
    std::make_shared<ncmv::level_filter>(ncmv::log_level::info)
);
```

---

## Extending the library

### Custom sink

```cpp
class file_sink final : public ncmv::i_log_sink {
public:
    explicit file_sink(const std::string& path)
        : file_(path, std::ios::app) {}

    void write(const std::string& record) override {
        file_ << record << '\n';
    }

private:
    std::ofstream file_;
};
```

### Custom formatter

```cpp
class json_formatter final : public ncmv::i_log_formatter {
public:
    std::string format(ncmv::log_level level,
                       long long elapsed_ms,
                       const std::string& message) const override {
        // build JSON string ...
    }
};
```

### Custom filter

```cpp
class sampling_filter final : public ncmv::i_log_filter {
public:
    bool is_enabled(ncmv::log_level /*level*/) const noexcept override {
        return (counter_++ % 10) == 0;  // emit every 10th record
    }
private:
    mutable int counter_{0};
};
```

---

## Build

```bash
g++ -std=c++14 -pthread -I.. -o test_logging test/test_logging.cpp && ./test_logging
```
