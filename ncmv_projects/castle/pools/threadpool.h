#pragma once

// ─────────────────────────────────────────────────────────────────────────────
// CASTLE thread pool — embedded-friendly, priority-scheduled worker pool.
//
// Design goals (vs. the previous std::string / verbose-log version):
//   • Bounded logging.  Every diagnostic message emitted by the pool is
//     assembled through castle::basic_logger<LogCapacity>, so the maximum
//     record size is a compile-time template parameter.  Long strings can
//     never blow the stack or the log sink budget on a resource-constrained
//     target — records above the cap are truncated with a trailing "...".
//   • Deterministic construction/teardown, no leaks: the singleton is a
//     Meyer's-style function-local static (no `new`, no manual delete, safe
//     under C++11 statics-init rules).
//   • snake_case throughout, matching the rest of the CASTLE codebase.
//   • Usability preserved: the priority-based enqueue<> API is unchanged
//     (except for naming), the convenience macros still work, and the
//     header is drop-in for existing translation units.
//
// C++17 required.
// ─────────────────────────────────────────────────────────────────────────────

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include "../logging/logging.h"

namespace castle
{

// ─────────────────────────────────────────────────────────────────────────────
// Compile-time defaults
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Default maximum length (in bytes, not counting the trailing NUL) of a single
 * diagnostic record emitted by the thread pool.  Kept small on purpose — the
 * pool only reports a handful of short lifecycle events (start / stop / drop),
 * never the payload of user tasks.
 */
inline constexpr std::size_t default_pool_log_capacity = 128;

// ─────────────────────────────────────────────────────────────────────────────
// basic_thread_pool<LogCapacity>
// ─────────────────────────────────────────────────────────────────────────────

/**
 * Priority-scheduled thread pool.
 *
 * Template parameter:
 *   LogCapacity — compile-time upper bound (bytes) for any single log record
 *                 emitted by the pool.  Forwarded to basic_logger<LogCapacity>
 *                 so no unbounded string is ever assembled.
 *
 * Tasks are ordered by (priority DESC, timestamp ASC): higher priority runs
 * first, ties are broken FIFO.
 *
 * Thread safety: fully thread-safe; enqueue() may be called concurrently
 * from any number of producer threads.
 */
template <std::size_t LogCapacity = default_pool_log_capacity>
class basic_thread_pool
{
public:
    static constexpr std::size_t log_capacity = LogCapacity;
    using logger_t = basic_logger<LogCapacity>;

    struct task
    {
        std::chrono::steady_clock::time_point timestamp;
        int                                   priority;
        std::function<void()>                 func;

        // Higher priority first; on ties, earlier-submitted first.
        // std::priority_queue is a max-heap, hence the inverted logic.
        bool operator<(const task& other) const noexcept
        {
            if (priority != other.priority)
                return priority < other.priority;
            return timestamp > other.timestamp;
        }
    };

    // ── Singleton access ──────────────────────────────────────────────────

    /**
     * Return the process-wide pool.  The very first call fixes the worker
     * count for the lifetime of the program; subsequent calls ignore
     * @p num_threads and return the same instance.
     *
     * Uses a Meyer's singleton — thread-safe under C++11, no manual delete,
     * no static-order fiasco, no leak.
     */
    static basic_thread_pool& get_instance(
        std::size_t num_threads = std::thread::hardware_concurrency()) noexcept
    {
        static basic_thread_pool instance_{ num_threads };
        return instance_;
    }

    ~basic_thread_pool()
    {
        {
            std::unique_lock<std::mutex> lock(tasks_mutex_);
            running_ = false;
        }
        condition_.notify_all();

        for (std::thread& worker : workers_)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }

        log_.log_info("thread_pool stopped");
    }

    basic_thread_pool(const basic_thread_pool&)            = delete;
    basic_thread_pool& operator=(const basic_thread_pool&) = delete;
    basic_thread_pool(basic_thread_pool&&)                 = delete;
    basic_thread_pool& operator=(basic_thread_pool&&)      = delete;

    // ── Producer API ──────────────────────────────────────────────────────

    /**
     * Submit a callable + its arguments for asynchronous execution.
     * Returns a std::future carrying the callable's result.
     *
     * @param priority  Higher values run sooner; ties resolved FIFO.
     */
    template <class F, class... Args>
    auto enqueue(int priority, F&& f, Args&&... args)
        -> std::future<typename std::invoke_result<F, Args...>::type>
    {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto packaged = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> result = packaged->get_future();

        task t{};
        t.timestamp = std::chrono::steady_clock::now();
        t.priority  = priority;
        t.func      = [packaged]() { (*packaged)(); };

        {
            std::unique_lock<std::mutex> lock(tasks_mutex_);
            if (!running_)
            {
                // Pool is shutting down — refuse the task deterministically.
                log_.log_warning("enqueue rejected: pool stopping");
                throw std::runtime_error("thread_pool: stopped");
            }
            tasks_.push(std::move(t));
        }

        condition_.notify_one();
        return result;
    }

    /** Number of worker threads owned by the pool. */
    std::size_t worker_count() const noexcept { return workers_.size(); }

    /** Access the pool's bounded logger (for injecting sinks/filters). */
    logger_t& log() noexcept { return log_; }

private:
    explicit basic_thread_pool(std::size_t num_threads)
        : running_(true)
    {
        if (num_threads == 0U)
        {
            num_threads = 1U;   // Guarantee forward progress on odd platforms.
        }

        workers_.reserve(num_threads);
        for (std::size_t i = 0U; i < num_threads; ++i)
        {
            workers_.emplace_back(&basic_thread_pool::worker_loop, this);
        }

        log_.log_info("thread_pool started, workers=", num_threads);
    }

    void worker_loop()
    {
        while (true)
        {
            task t;
            {
                std::unique_lock<std::mutex> lock(tasks_mutex_);
                condition_.wait(lock, [this]() {
                    return !running_ || !tasks_.empty();
                });

                if (!running_ && tasks_.empty())
                {
                    break;
                }

                t = tasks_.top();
                tasks_.pop();
            }

            if (t.func)
            {
                try
                {
                    t.func();
                }
                catch (...)
                {
                    // Swallow — a rogue task must never take down a worker.
                    log_.log_error("task threw, swallowed");
                }
            }
        }
    }

    std::atomic<bool>              running_;
    std::vector<std::thread>       workers_;
    std::priority_queue<task>      tasks_;
    std::mutex                     tasks_mutex_;
    std::condition_variable        condition_;
    logger_t                       log_;
};

/** Convenience alias: default-capacity thread pool used by the macros. */
using thread_pool = basic_thread_pool<default_pool_log_capacity>;

} // namespace castle

// ─────────────────────────────────────────────────────────────────────────────
// Convenience macros — mirror the pre-refactor public surface.
//
//   INITIALIZE_THREAD_POOL(n)  — construct-on-first-use with @p n workers.
//   GET_THREAD_POOL()          — retrieve the already-initialised pool.
//
// Both resolve to a reference (not a pointer): call methods with `.` and
// never worry about lifetime — the pool is destroyed at program exit.
// ─────────────────────────────────────────────────────────────────────────────

#define INITIALIZE_THREAD_POOL(n) ::castle::thread_pool::get_instance((n))
#define GET_THREAD_POOL()         ::castle::thread_pool::get_instance()
