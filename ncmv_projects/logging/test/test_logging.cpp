/**
 * @file  test_logging.cpp
 * @brief Usage examples for ncmv::logger (logging.hpp).
 *
 * Build (C++14):
 *   g++ -std=c++14 -pthread -I.. -o test_logging test_logging.cpp && ./test_logging
 */

#include "logging.hpp"

#include <fstream>
#include <sstream>
#include <thread>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
// 1. Custom formatter  — demonstrates OCP / DIP  (inject a new formatter)
// ─────────────────────────────────────────────────────────────────────────────
class csv_formatter final : public ncmv::i_log_formatter {
public:
    // Produces: elapsed_ms,LEVEL,message
    std::string format(ncmv::log_level    level,
                       long long          elapsed_ms,
                       const std::string& message) const override {
        std::ostringstream oss;
        oss << elapsed_ms << ',';
        switch (level) {
            case ncmv::log_level::debug:   oss << "DEBUG";   break;
            case ncmv::log_level::info:    oss << "INFO";    break;
            case ncmv::log_level::warning: oss << "WARNING"; break;
            case ncmv::log_level::error:   oss << "ERROR";   break;
            default:                       oss << "UNKNOWN"; break;
        }
        oss << ',' << message;
        return oss.str();
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// 2. Custom sink  — demonstrates OCP / DIP  (inject a new sink)
// ─────────────────────────────────────────────────────────────────────────────
class string_sink final : public ncmv::i_log_sink {
public:
    void write(const std::string& record) override {
        buffer_ += record + '\n';
    }

    const std::string& buffer() const noexcept { return buffer_; }

private:
    std::string buffer_;
};

// ─────────────────────────────────────────────────────────────────────────────
// helpers
// ─────────────────────────────────────────────────────────────────────────────
static void section(const char* title) {
    std::cout << "\n── " << title << " ──\n";
}

// ─────────────────────────────────────────────────────────────────────────────
int main() {

    // ── Basic usage ──────────────────────────────────────────────────────────
    section("1. Basic usage — default logger (stdout)");
    {
        ncmv::logger lg;

        lg.log(ncmv::log_level::info, "Application started");
        lg.log_debug("pi ≈ ", 3.14159, "  e ≈ ", 2.71828);
        lg.log_info("User '", "alice", "' logged in, session=", 42);
        lg.log_warning("Disk usage at ", 87, "%");
        lg.log_error("Connection refused on port ", 8080);
    }

    // ── Log level filtering ──────────────────────────────────────────────────
    section("2. Level filtering — only WARNING and above");
    {
        ncmv::logger lg;
        lg.set_min_level(ncmv::log_level::warning);

        lg.log_debug("This debug record is filtered out");     // suppressed
        lg.log_info("This info record is also filtered out");  // suppressed
        lg.log_warning("Low memory: ", 128, " MB remaining");  // printed
        lg.log_error("Out-of-memory condition detected");      // printed
    }

    // ── Custom ostream ───────────────────────────────────────────────────────
    section("3. Custom ostream — writing to std::cerr");
    {
        ncmv::logger lg(std::cerr);
        lg.log_error("Fatal error reported on stderr");
    }

    // ── Injecting a custom formatter ─────────────────────────────────────────
    section("4. Custom formatter — CSV output");
    {
        ncmv::logger lg;
        lg.set_formatter(std::make_shared<csv_formatter>());

        lg.log_info("csv,with,commas,is,fine");
        lg.log_warning("value=", 99);
    }

    // ── Full dependency injection ─────────────────────────────────────────────
    section("5. Full DI — in-memory sink for testing");
    {
        auto sink      = std::make_shared<string_sink>();
        auto formatter = std::make_shared<ncmv::default_formatter>();
        auto filter    = std::make_shared<ncmv::level_filter>(ncmv::log_level::info);

        ncmv::logger lg(sink, formatter, filter);
        lg.log_debug("not captured");            // filtered
        lg.log_info("captured: value=", 7);      // passes filter

        std::cout << "Captured output:\n" << sink->buffer();
    }

    // ── Thread safety ────────────────────────────────────────────────────────
    section("6. Concurrent logging from multiple threads");
    {
        ncmv::logger lg;
        lg.set_min_level(ncmv::log_level::info);

        constexpr int thread_count = 4;
        constexpr int msgs_per_thread = 3;

        std::vector<std::thread> workers;
        workers.reserve(thread_count);

        for (int t = 0; t < thread_count; ++t) {
            workers.emplace_back([&lg, t]() {
                for (int m = 0; m < msgs_per_thread; ++m) {
                    lg.log_info("thread=", t, "  msg=", m);
                }
            });
        }

        for (auto& w : workers) w.join();
    }

    // ── Disable all output ───────────────────────────────────────────────────
    section("7. Silence everything with log_level::none");
    {
        ncmv::logger lg;
        lg.set_min_level(ncmv::log_level::none);
        lg.log_error("This is not printed");   // silenced
        std::cout << "(no output expected above)\n";
    }

    return 0;
}
