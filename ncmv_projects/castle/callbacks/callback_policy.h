#pragma once

#include <atomic>
#include <chrono>
#include <cstddef>
#include <optional>
#include <type_traits>
#include <utility>

// =============================================================================
// castle::callback_policy
// -----------------------------------------------------------------------------
// A collection of small, header-only "control-flow" policies that decide *when*
// a bound callback should actually run. Designed for embedded / resource
// constrained C++17 code:
//
//   * Zero heap allocation. The callable is stored BY VALUE via a template
//     parameter. No std::function, no shared_ptr, no
//     std::unique_ptr, no owned threads or timers.
//   * Bind once. The callable is captured at construction and never rebound;
//     execute() only forwards its call-site arguments.
//   * Thread-safety is opt-in per policy: single_thread (no synchronization,
//     default) or concurrent (atomic-based, no mutex).
//   * Time-based policies (throttle, periodic) are poll-driven: they expose an
//     execute()/poll() method that the caller pumps from a cooperative event
//     loop. They own no threads.
//
// Interoperability with castle::events::function.h / inplace_function.h
// -----------------------------------------------------------------------------
// The Callback template parameter can be anything invocable. Recommended:
//
//   1) A raw callable (lambda, functor, function pointer). Zero indirection,
//      the closure type is inlined into the policy's storage.
//
//        auto init = callback_policy::once::make_policy_st([]{ boot_hw(); });
//
//   2) A castle::inplace_function<Sig, N> for type erasure with SBO. Still no
//      heap allocation. Useful when need a stable stored type.
//
//        using cb_t = inplace_function<void()>;
//        callback_policy::once::single_thread<cb_t> init{ cb_t{ big_lambda } };
//
//   3) One of castle::events::function_ct / function_ct_m / function_ct_im for
//      compile-time bound targets - potentially zero storage per policy.
//
// API convention
// -----------------------------------------------------------------------------
// Every policy exposes a make_policy_* factory that deduces the callable type;
// the client binds it to a local with `auto`:
//
//   auto init = callback_policy::once::make_policy_st([]{ boot_hw(); });
//   init.execute();
//
// Naming legend used throughout this header
// -----------------------------------------------------------------------------
//   * make_policy_st          - factory producing a `single_thread` policy
//                        ("st" = single-thread, non-synchronized, cheapest).
//   * make_policy_concurrent  - factory producing a `concurrent` policy
//                        (atomic-based, safe to call execute() from multiple
//                         threads; no mutex, no heap).
//   * make_policy_ct<N>       - factory producing a policy with a compile-time
//                        constant ("ct" = compile-time) parameter such as N
//                        for every_n. The value is a template argument, so
//                        the compiler can strength-reduce operations on it.
//   * make_policy_st_with_clock<Clock> - variant of make_policy_st that lets the caller
//                        override the default std::chrono::steady_clock.
// =============================================================================

namespace castle
{
namespace callbacks
{
namespace policy
{

// -----------------------------------------------------------------------------
// once - fire the callback exactly once during the object's lifetime.
// -----------------------------------------------------------------------------
namespace once
{

// Non-thread-safe variant. Use when execute() is only ever called from one
// thread (typical for init sequences, feature-gates, first-frame hooks, ...).
template <typename Callback>
class single_thread
{
public:
    using callback_type = Callback;

    template <typename C, typename = std::enable_if_t<!std::is_same<std::decay_t<C>, single_thread>::value>>
    explicit single_thread(C&& cb) noexcept(std::is_nothrow_constructible<Callback, C&&>::value)
        : cb_(std::forward<C>(cb))
    {
    }

    // Returns true iff the callback was actually invoked by this call.
    template <typename... Args>
    bool execute(Args&&... args)
    {
        if (fired_)
        {
            return false;
        }
        fired_ = true;  // set before invoke so recursive execute() is a no-op
        cb_(std::forward<Args>(args)...);
        return true;
    }

    void reset() noexcept { fired_ = false; }
    bool has_fired() const noexcept { return fired_; }

private:
    Callback cb_;
    bool     fired_ = false;
};


// Concurrent variant: atomic CAS makes at most one caller win the race.
// No mutex, no dynamic allocation.
template <typename Callback>
class concurrent
{
public:
    using callback_type = Callback;

    template <typename C, typename = std::enable_if_t<!std::is_same<std::decay_t<C>, concurrent>::value>>
    explicit concurrent(C&& cb) noexcept(std::is_nothrow_constructible<Callback, C&&>::value)
        : cb_(std::forward<C>(cb))
    {
    }

    template <typename... Args>
    bool execute(Args&&... args)
    {
        bool expected = false;
        if (!fired_.compare_exchange_strong(expected, true, std::memory_order_acq_rel, std::memory_order_acquire))
        {
            return false;
        }
        cb_(std::forward<Args>(args)...);
        return true;
    }

    void reset() noexcept
    {
        fired_.store(false, std::memory_order_release);
    }

    bool has_fired() const noexcept
    {
        return fired_.load(std::memory_order_acquire);
    }

private:
    Callback          cb_;
    std::atomic<bool> fired_{false};
};


// Factories: deduce the callable type from the argument.
//
//   make_policy_st         -> single_thread variant (no synchronization).
//   make_policy_concurrent -> concurrent variant (atomic CAS, thread-safe).
//
//   auto init = callback_policy::once::make_policy_st([]{ boot(); });
//   auto flag = callback_policy::once::make_policy_concurrent([]{ log_once(); });
template <typename C>
single_thread<typename std::decay<C>::type> make_policy_st(C&& cb)
{
    return single_thread<typename std::decay<C>::type>(std::forward<C>(cb));
}

template <typename C>
concurrent<typename std::decay<C>::type> make_policy_concurrent(C&& cb)
{
    return concurrent<typename std::decay<C>::type>(std::forward<C>(cb));
}

} // namespace once


// -----------------------------------------------------------------------------
// every_n - fire the callback once every N execute() calls.
// -----------------------------------------------------------------------------
namespace every_n
{

template <typename Callback>
class single_thread
{
public:
    using callback_type = Callback;

    template <typename C, typename = std::enable_if_t<!std::is_same<std::decay_t<C>, single_thread>::value>>
    single_thread(std::size_t n, C&& cb)
        : cb_(std::forward<C>(cb))
        , n_(n == 0U ? 1U : n)
    {
    }

    template <typename... Args>
    bool execute(Args&&... args)
    {
        if (++counter_ < n_)
        {
            return false;
        }
        counter_ = 0U;
        cb_(std::forward<Args>(args)...);
        return true;
    }

    void reset() noexcept { counter_ = 0U; }
    std::size_t interval() const noexcept { return n_; }

private:
    Callback     cb_;
    std::size_t  n_;
    std::size_t  counter_ = 0U;
};


template <typename Callback>
class concurrent
{
public:
    using callback_type = Callback;

    template <typename C, typename = std::enable_if_t<!std::is_same<std::decay_t<C>, concurrent>::value>>
    concurrent(std::size_t n, C&& cb)
        : cb_(std::forward<C>(cb))
        , n_(n == 0U ? 1U : n)
    {
    }

    template <typename... Args>
    bool execute(Args&&... args)
    {
        const std::size_t prev = counter_.fetch_add(1U, std::memory_order_relaxed);
        if (((prev + 1U) % n_) != 0U)
        {
            return false;
        }
        cb_(std::forward<Args>(args)...);
        return true;
    }

    void reset() noexcept { counter_.store(0U, std::memory_order_relaxed); }
    std::size_t interval() const noexcept { return n_; }

private:
    Callback                 cb_;
    std::size_t              n_;
    std::atomic<std::size_t> counter_{0U};
};


// Factories with runtime N (N is a constructor argument, not a template arg).
//
//   make_policy_st         -> single_thread variant.
//   make_policy_concurrent -> concurrent variant (atomic counter).
//
//   auto hb = callback_policy::every_n::make_policy_st(100, []{ toggle_led(); });
template <typename C>
single_thread<typename std::decay<C>::type> make_policy_st(std::size_t n, C&& cb)
{
    return single_thread<typename std::decay<C>::type>(n, std::forward<C>(cb));
}

template <typename C>
concurrent<typename std::decay<C>::type> make_policy_concurrent(std::size_t n, C&& cb)
{
    return concurrent<typename std::decay<C>::type>(n, std::forward<C>(cb));
}

// Factory with a compile-time constant N ("ct" = compile-time). Prefer this
// when the ratio is fixed at build time: the modulo becomes a strength-reduced
// immediate for power-of-two values.
//
//   auto stats = callback_policy::every_n::make_policy_ct<64>([]{ dump_stats(); });
template <std::size_t N, typename C>
single_thread<typename std::decay<C>::type> make_policy_ct(C&& cb)
{
    static_assert(N > 0, "every_n::make_policy_ct requires N > 0");
    return single_thread<typename std::decay<C>::type>(N, std::forward<C>(cb));
}

} // namespace every_n


// -----------------------------------------------------------------------------
// on_change - fire the callback only when the observed value differs from
// the previously stored one. State is stored in-place via std::optional.
// -----------------------------------------------------------------------------
namespace on_change
{

template <typename T, typename Callback>
class single_thread
{
    static_assert(!std::is_reference<T>::value,
                  "on_change::single_thread requires a value type for T");
public:
    using value_type    = T;
    using callback_type = Callback;

    // Construct without an initial value; the first execute() always fires.
    template <typename C, typename = std::enable_if_t<!std::is_same<std::decay_t<C>, single_thread>::value>>
    explicit single_thread(C&& cb)
        : cb_(std::forward<C>(cb))
    {
    }

    // Construct with an initial value; execute(v) fires only if v != initial.
    template <typename V, typename C>
    single_thread(V&& initial, C&& cb)
        : cb_(std::forward<C>(cb))
        , last_(std::in_place, std::forward<V>(initial))
    {
    }

    template <typename U>
    bool execute(U&& new_value)
    {
        if (last_.has_value() && (*last_ == new_value))
        {
            return false;
        }
        last_.emplace(std::forward<U>(new_value));
        cb_(*last_);
        return true;
    }

    void reset() noexcept(std::is_nothrow_destructible<T>::value)
    {
        last_.reset();
    }

    bool has_value() const noexcept { return last_.has_value(); }

private:
    Callback         cb_;
    std::optional<T> last_;
};


// Factory with initial value; T is deduced from `initial`
// ("st" = single_thread variant).
//
//   auto w = callback_policy::on_change::make_policy_st(23, [](int v){ ... });
template <typename V, typename C>
single_thread<typename std::decay<V>::type, typename std::decay<C>::type>
make_policy_st(V&& initial, C&& cb)
{
    return single_thread<typename std::decay<V>::type,
                         typename std::decay<C>::type>(
        std::forward<V>(initial),
        std::forward<C>(cb)
    );
}

// Factory without initial value; T must be given explicitly.
//
//   auto w = callback_policy::on_change::make_policy_st<int>([](int v){ ... });
template <typename T, typename C>
single_thread<T, typename std::decay<C>::type> make_policy_st(C&& cb)
{
    return single_thread<T, typename std::decay<C>::type>(std::forward<C>(cb));
}

} // namespace on_change


// -----------------------------------------------------------------------------
// throttle - rate-limit. Fire only if `interval` has elapsed since the last
// fire; extra invocations within the interval are dropped (not queued).
// Poll-driven: no thread, no timer. Suitable for main-loop pumping.
// -----------------------------------------------------------------------------
namespace throttle
{

template <typename Callback, typename Clock = std::chrono::steady_clock>
class single_thread
{
public:
    using callback_type = Callback;
    using clock         = Clock;
    using duration      = typename Clock::duration;
    using time_point    = typename Clock::time_point;

    template <typename Rep, typename Period, typename C,
              typename = std::enable_if_t<!std::is_same<std::decay_t<C>, single_thread>::value>>
    single_thread(std::chrono::duration<Rep, Period> interval, C&& cb)
        : cb_(std::forward<C>(cb))
        , interval_(std::chrono::duration_cast<duration>(interval))
    {
    }

    template <typename... Args>
    bool execute(Args&&... args)
    {
        const time_point now = Clock::now();
        if (last_.has_value() && (now - *last_) < interval_)
        {
            return false;
        }
        last_ = now;
        cb_(std::forward<Args>(args)...);
        return true;
    }

    // Force next execute() to fire regardless of the elapsed interval.
    void reset() noexcept { last_.reset(); }

    duration interval() const noexcept { return interval_; }

private:
    Callback                  cb_;
    duration                  interval_;
    std::optional<time_point> last_;
};


// Factory using the default clock (std::chrono::steady_clock)
// ("st" = single_thread variant).
//
//   using namespace std::chrono_literals;
//   auto gate = callback_policy::throttle::make_policy_st(500ms,
//                   [](const char* m){ log(m); });
template <typename Rep, typename Period, typename C>
single_thread<typename std::decay<C>::type>
make_policy_st(std::chrono::duration<Rep, Period> interval, C&& cb)
{
    return single_thread<typename std::decay<C>::type>(interval, std::forward<C>(cb));
}

// Same as make_policy_st but with a user-supplied clock type.
template <typename Clock, typename Rep, typename Period, typename C>
single_thread<typename std::decay<C>::type, Clock>
make_policy_st_with_clock(std::chrono::duration<Rep, Period> interval, C&& cb)
{
    return single_thread<typename std::decay<C>::type, Clock>(interval, std::forward<C>(cb));
}

} // namespace throttle


// -----------------------------------------------------------------------------
// periodic - fire every `period` when the user pumps the policy. Poll-driven,
// with catch-up: if several periods elapsed between polls, the callback runs
// that many times so the schedule does not drift.
// -----------------------------------------------------------------------------
namespace periodic
{

template <typename Callback, typename Clock = std::chrono::steady_clock>
class single_thread
{
public:
    using callback_type = Callback;
    using clock         = Clock;
    using duration      = typename Clock::duration;
    using time_point    = typename Clock::time_point;

    template <typename Rep, typename Period, typename C,
              typename = std::enable_if_t<!std::is_same<std::decay_t<C>, single_thread>::value>>
    single_thread(std::chrono::duration<Rep, Period> period, C&& cb)
        : cb_(std::forward<C>(cb))
        , period_(std::chrono::duration_cast<duration>(period))
        , next_deadline_(Clock::now() + period_)
    {
    }

    // Advance the internal deadline in fixed steps and call the callback for
    // each step that has elapsed. Returns the number of fires performed.
    template <typename... Args>
    std::size_t poll(Args&&... args)
    {
        std::size_t fires = 0U;
        const time_point now = Clock::now();
        while (now >= next_deadline_)
        {
            cb_(std::forward<Args>(args)...);
            next_deadline_ += period_;
            ++fires;
        }
        return fires;
    }

    // Snap the next deadline to now + period (discards accumulated lag).
    void reset() noexcept { next_deadline_ = Clock::now() + period_; }

    duration   period() const noexcept        { return period_; }
    time_point next_deadline() const noexcept { return next_deadline_; }

private:
    Callback   cb_;
    duration   period_;
    time_point next_deadline_;
};


// Factory using the default clock (std::chrono::steady_clock)
// ("st" = single_thread variant).
//
//   using namespace std::chrono_literals;
//   auto tick = callback_policy::periodic::make_policy_st(1s, []{ housekeeping(); });
template <typename Rep, typename Period, typename C>
single_thread<typename std::decay<C>::type>
make_policy_st(std::chrono::duration<Rep, Period> period, C&& cb)
{
    return single_thread<typename std::decay<C>::type>(period, std::forward<C>(cb));
}

// Same as make_policy_st but with a user-supplied clock type.
template <typename Clock, typename Rep, typename Period, typename C>
single_thread<typename std::decay<C>::type, Clock>
make_policy_st_with_clock(std::chrono::duration<Rep, Period> period, C&& cb)
{
    return single_thread<typename std::decay<C>::type, Clock>(period, std::forward<C>(cb));
}

} // namespace periodic

} // namespace policy
} // namespace callbacks
} // namespace castle