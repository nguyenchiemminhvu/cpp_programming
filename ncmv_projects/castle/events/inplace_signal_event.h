#pragma once

#include "../callbacks/inplace_function.h"
#include "../callbacks/inplace_callback_registry.h"
#include "signal_config.h"

#include <array>
#include <atomic>
#include <bitset>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

using castle::callbacks::inplace_function;
using castle::callbacks::inplace_callback_registry;
using castle::callbacks::inplace_callback_registry_error;
using castle::callbacks::inplace_callback_subscription;

namespace castle
{
namespace events
{

// -----------------------------------------------------------------------------
// Error codes returned by inplace_signal_event operations.
// -----------------------------------------------------------------------------
enum class inplace_signal_event_error : std::uint8_t
{
    ok = 0,
    full,
    invalid_callback,
    invalid_subscription,
    signal_disabled,
    install_failed
};

// -----------------------------------------------------------------------------
// inplace_signal_event - compile-time, signum-keyed OS signal dispatcher
// built on top of inplace_callback_registry (owning callbacks).
//
// Design (all-static, namespace-shaped):
//   - The set of signals handled is fixed at compile time via a pack of
//     castle::events::signal enumerators. Each signal is a compile-time
//     key resolved to an O(1) array index — no runtime search on the hot
//     paths, no hashing, no heap.
//   - Every signal owns its own inplace_callback_registry<
//         MaxCallbackPerSignal, void(),
//         CallbackStorageSize, CallbackStorageAlignment>.
//     The registry stores each subscribed callable by value in a fixed
//     inline buffer, so no heap allocation happens even for stateful
//     lambdas. The signal identity is already encoded at register-time
//     (Signum template argument), so callbacks take no runtime arguments.
//
// Immediate dispatch model:
//   - When a subscribed signal is delivered by the OS, the installed
//     handler walks the matching registry and invokes every active
//     callback DIRECTLY from signal context. There is no deferred queue,
//     no dispatch_pending() step.
//   - The library-owned dispatch path is intentionally minimal: an array
//     walk, an active-flag check, and one indirect call through the
//     inplace_function's stored function pointer — no allocation, no
//     locking, no stdio, no re-entrancy on any library-owned mutex.
//   - A single atomic "ready" gate synchronises setup with signal
//     delivery: install() is the release barrier that publishes every
//     prior register_callback() to the OS handler; the handler performs
//     an acquire-load and bails out if setup is incomplete or if
//     uninstall() has already begun tearing down. This closes the window
//     where a signal fired between arming sigaction and finishing user
//     setup could observe half-initialised registry state, especially
//     when the signal is delivered to a different thread than the one
//     doing setup (POSIX allows arbitrary thread delivery).
//
//   *** ASYNC-SIGNAL-SAFETY WARNING ***
//   Because callbacks execute in signal context, THE USER CALLBACK BODY
//   MUST ITSELF BE ASYNC-SIGNAL-SAFE. In practice this means:
//     - No malloc / new / delete (avoid std::string, std::vector, ...).
//     - No mutex / condition variable / any locking primitive that could
//       already be held by the interrupted thread.
//     - No stdio (printf, std::cout, ...). Use write(2) on STDERR_FILENO
//       if you need diagnostics.
//     - Only the functions listed as async-signal-safe by POSIX
//       (signal-safety(7)) may be called.
//   The inplace_function machinery itself is neutral: subscription only
//   copies/moves the callable into its inline buffer at register time
//   (which does not run in signal context), and dispatch is a single
//   indirect call. The unsafe part is whatever code you write inside
//   the target. If you cannot meet these constraints, do NOT use this
//   handler directly — instead have the callback set a std::atomic_flag
//   / write a byte to a self-pipe and do the real work from a normal
//   thread.
//
// Template parameters:
//   MaxCallbackPerSignal      - fixed subscriber capacity per signal.
//   CallbackStorageSize       - inplace_function inline buffer size in
//                               bytes. Must be >= sizeof of the largest
//                               captured callable the user will subscribe
//                               (a static_assert inside inplace_function
//                               fires otherwise). Default 64 matches the
//                               inplace_function default.
//   CallbackStorageAlignment  - inplace_function inline buffer alignment.
//                               Default alignof(std::max_align_t).
//   Signals...                - the OS signals to handle, expressed as
//                               values of the strongly-typed enum
//                               castle::events::signal (signal::sigint,
//                               signal::sigterm, ...).
//
// Usage:
//   using castle::events::signal;
//   using signals_t = castle::events::inplace_signal_event<
//       4,                              // max callbacks per signal
//       64,                             // callback storage size
//       alignof(std::max_align_t),      // callback storage alignment
//       signal::sigint,
//       signal::sigterm>;
//
//   signals_t::install();
//
//   // Lambda with capture — owned by the registry, no external storage.
//   volatile std::sig_atomic_t stop = 0;
//   auto sub = signals_t::register_callback<signal::sigint>(
//       [&stop]() { stop = 1; });
//
//   // ... run the program; callbacks fire directly from signal context ...
//
//   sub.unsubscribe();
//   signals_t::uninstall();
// -----------------------------------------------------------------------------
template <
    std::size_t MaxCallbackPerSignal,
    std::size_t CallbackStorageSize,
    std::size_t CallbackStorageAlignment,
    signal... Signals>
class inplace_signal_event
{
    static_assert(sizeof...(Signals) > 0,
                  "inplace_signal_event requires at least one signal");
    static_assert(MaxCallbackPerSignal > 0,
                  "inplace_signal_event requires MaxCallbackPerSignal > 0");
    static_assert(CallbackStorageSize > 0,
                  "inplace_signal_event requires CallbackStorageSize > 0");
    static_assert(CallbackStorageAlignment > 0,
                  "inplace_signal_event requires CallbackStorageAlignment > 0");

    // -------------------------------------------------------------------------
    // Compile-time uniqueness check for the Signals... pack.
    //
    // Duplicates would silently break the design:
    //   - sigaction() would be armed twice for the same signum (only the
    //     last arming counts, and the "wasted" slot leaks capacity),
    //   - index_of_runtime() always returns the FIRST match, so enable /
    //     disable / clear operations on the duplicate slot would be
    //     unreachable from the OS handler side,
    //   - subscriber_count() and callback_capacity() would report values
    //     that no longer reflect the real per-signal budget.
    // -------------------------------------------------------------------------
    template <signal... Pack>
    struct signals_are_unique;

    template <>
    struct signals_are_unique<> : std::true_type {};

    template <signal Head, signal... Tail>
    struct signals_are_unique<Head, Tail...>
    {
        static constexpr bool value = ((Head != Tail) && ...) && signals_are_unique<Tail...>::value;
    };

    static_assert(signals_are_unique<Signals...>::value,
                  "inplace_signal_event: the Signals... pack must not contain duplicates");

public:
    using error = inplace_signal_event_error;
    using subscription = inplace_callback_subscription;
    using registry_type = inplace_callback_registry<
        MaxCallbackPerSignal,
        void(),
        CallbackStorageSize,
        CallbackStorageAlignment>;
    using callback_type = typename registry_type::callback_type;

    static constexpr std::size_t signal_count = sizeof...(Signals);

    // The class is a pure static namespace — no instances, ever. Enforcing
    // this at the type level is clearer than a "singleton runtime check".
    inplace_signal_event() = delete;
    ~inplace_signal_event() = delete;
    inplace_signal_event(const inplace_signal_event&) = delete;
    inplace_signal_event& operator=(const inplace_signal_event&) = delete;
    inplace_signal_event(inplace_signal_event&&) = delete;
    inplace_signal_event& operator=(inplace_signal_event&&) = delete;

    // -------------------------------------------------------------------------
    // Compile-time capacity queries.
    // -------------------------------------------------------------------------
    static constexpr std::size_t signal_capacity() noexcept
    {
        return signal_count;
    }

    static constexpr std::size_t callback_capacity() noexcept
    {
        return MaxCallbackPerSignal;
    }

    static constexpr std::size_t callback_storage_size() noexcept
    {
        return CallbackStorageSize;
    }

    static constexpr std::size_t callback_storage_alignment() noexcept
    {
        return CallbackStorageAlignment;
    }

    // -------------------------------------------------------------------------
    // Install the OS-level handler for every signal in the pack. Signals
    // start enabled. Safe to call multiple times — subsequent calls simply
    // re-arm sigaction with the same handler.
    //
    // Ordering contract: any register_callback() / enable_signal() /
    // clear_signal() call sequenced-before install() is guaranteed to be
    // visible to the OS handler on any thread (release-store publishes
    // the setup; the handler's acquire-load synchronises with it).
    // Registrations issued AFTER install() are inherently racy against
    // in-flight signal delivery and should be treated as single-writer.
    // -------------------------------------------------------------------------
    static error install() noexcept
    {
        // Force any concurrent os_handler invocation to bail before we
        // touch enabled_ / sigaction state.
        ready_.store(false, std::memory_order_release);

        enabled_.set();

        struct sigaction sa{};
        sa.sa_handler = &inplace_signal_event::os_handler;
        sigemptyset(&sa.sa_mask);
        // SA_RESTART: transparently restart interrupted syscalls in user
        // code; the library never blocks in the handler itself.
        sa.sa_flags = SA_RESTART;

        for (std::size_t i = 0; i < signal_count; ++i)
        {
            if (::sigaction(signal_list_[i], &sa, nullptr) != 0)
            {
                // Roll back so the process is not left in a half-armed
                // state (some signals delivered to our handler, others
                // still to whatever was previously installed).
                uninstall();
                return error::install_failed;
            }
        }

        // Release: publishes all prior setup (registries, enable flags,
        // sigaction arming) to any thread that observes ready_ == true
        // via the matching acquire-load in os_handler.
        ready_.store(true, std::memory_order_release);
        return error::ok;
    }

    // -------------------------------------------------------------------------
    // Restore SIG_DFL for every managed signal. Safe to call multiple
    // times. Existing subscriptions are kept in their registries (so a
    // subsequent install() re-arms transparently); call clear() first if
    // you also want to drop them.
    //
    // Ordering: the ready gate is closed BEFORE sigaction is rewound, so
    // an in-flight os_handler on another thread observes ready_ == false
    // via its acquire-load and bails out without touching the registries.
    // -------------------------------------------------------------------------
    static void uninstall() noexcept
    {
        // Close the gate first so any concurrent handler bails before we
        // start mutating OS-level dispositions.
        ready_.store(false, std::memory_order_release);

        struct sigaction sa{};
        sa.sa_handler = SIG_DFL;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;

        for (std::size_t i = 0; i < signal_count; ++i)
        {
            // Ignore failure — no meaningful recovery available and no user
            // callback context to report to.
            (void)::sigaction(signal_list_[i], &sa, nullptr);
        }
    }

    static bool is_installed() noexcept
    {
        return ready_.load(std::memory_order_acquire);
    }

    // -------------------------------------------------------------------------
    // Register a callback for the given signal. Signature is void() — the
    // signal identity is already fixed by the Signum template argument, so
    // no runtime payload is passed.
    //
    // The registry OWNS the callback (copied/moved into the inplace_function
    // inline buffer), so lambdas with captures, functor objects, and
    // std::bind results can all be subscribed without external lifetime
    // management. The callable must satisfy inplace_function's size /
    // alignment constraints (CallbackStorageSize / CallbackStorageAlignment
    // — checked by a static_assert inside inplace_function).
    //
    // NOTE: the callback body will execute in signal context — see the
    // async-signal-safety warning at the top of this file.
    // -------------------------------------------------------------------------
    template <signal Signum, typename Callable>
    static subscription register_callback(Callable&& callback, error* out_error = nullptr)
    {
        constexpr std::size_t idx = index_of<Signum>();
        inplace_callback_registry_error inner_error = inplace_callback_registry_error::ok;

        subscription sub = registries_[idx].subscribe(std::forward<Callable>(callback), &inner_error);

        if (out_error != nullptr)
        {
            *out_error = convert_error(inner_error);
        }

        return sub;
    }

    // -------------------------------------------------------------------------
    // Per-signal enable / disable / query. A disabled signal still runs
    // the OS handler (we cannot inhibit delivery from user space cheaply)
    // but the handler returns without invoking any callback. Useful for
    // temporary suppression without touching sigaction / sigprocmask.
    // -------------------------------------------------------------------------
    template <signal Signum>
    static void enable_signal() noexcept
    {
        enabled_.set(index_of<Signum>());
    }

    template <signal Signum>
    static void disable_signal() noexcept
    {
        enabled_.reset(index_of<Signum>());
    }

    template <signal Signum>
    static bool is_signal_enabled() noexcept
    {
        return enabled_.test(index_of<Signum>());
    }

    // -------------------------------------------------------------------------
    // Clear subscriptions for one signal / all signals. Outstanding
    // subscription handles for cleared slots become stale.
    // -------------------------------------------------------------------------
    template <signal Signum>
    static void clear_signal() noexcept
    {
        registries_[index_of<Signum>()].clear();
    }

    static void clear() noexcept
    {
        for (std::size_t i = 0; i < signal_count; ++i)
        {
            registries_[i].clear();
        }
    }

    template <signal Signum>
    static std::size_t subscriber_count() noexcept
    {
        return registries_[index_of<Signum>()].size();
    }

private:
    // -------------------------------------------------------------------------
    // Compile-time index of a signal inside the Signals... pack.
    // Recursion is used instead of C++17 fold expressions so the logic
    // stays readable and produces a clean static_assert on unknown values.
    // -------------------------------------------------------------------------
    template <signal Target, std::size_t I, signal First, signal... Rest>
    struct index_of_impl
    {
        static constexpr std::size_t value = (Target == First)
                                             ? I
                                             : index_of_impl<Target, I + 1, Rest...>::value;
    };

    template <signal Target, std::size_t I, signal Last>
    struct index_of_impl<Target, I, Last>
    {
        static_assert(Target == Last, "inplace_signal_event: signal is not present in the Signals pack");
        static constexpr std::size_t value = I;
    };

    template <signal Signum>
    static constexpr std::size_t index_of() noexcept
    {
        return index_of_impl<Signum, 0, Signals...>::value;
    }

    // Runtime lookup used *only* from the async-signal handler. A tiny
    // linear scan over signal_list_ — async-signal-safe by construction
    // (no library calls, no allocation, just integer comparisons).
    static std::size_t index_of_runtime(int signum) noexcept
    {
        for (std::size_t i = 0; i < signal_count; ++i)
        {
            if (signal_list_[i] == signum)
            {
                return i;
            }
        }
        return signal_count; // sentinel: unknown signal
    }

    // -------------------------------------------------------------------------
    // OS-installed handler. Runs in signal context and dispatches
    // callbacks IMMEDIATELY. The library-owned portion of this path is
    // strictly async-signal-safe: array indexing, a bitset test, and a
    // walk of inplace_function slots invoked via a stored function
    // pointer. It never allocates, never locks, never touches stdio.
    // User callback bodies must uphold the same discipline (see
    // file-level warning).
    // -------------------------------------------------------------------------
    static void os_handler(int signum) noexcept
    {
        // Acquire: pairs with the release-store in install(). If setup is
        // not yet complete (or uninstall() has begun), bail out before
        // touching any registry state.
        if (!ready_.load(std::memory_order_acquire))
        {
            return;
        }

        const std::size_t idx = index_of_runtime(signum);
        if (idx >= signal_count)
        {
            return;
        }

        if (!enabled_.test(idx))
        {
            return;
        }

        registries_[idx].invoke();
    }

    // Map inplace_callback_registry_error to inplace_signal_event_error.
    static constexpr error convert_error(inplace_callback_registry_error error_code) noexcept
    {
        switch (error_code)
        {
            case inplace_callback_registry_error::ok:
            {
                return error::ok;
            }
            case inplace_callback_registry_error::full:
            {
                return error::full;
            }
            case inplace_callback_registry_error::invalid_callback:
            {
                return error::invalid_callback;
            }
            case inplace_callback_registry_error::invalid_subscription:
            {
                return error::invalid_subscription;
            }
        }
        return error::invalid_subscription;
    }

    // -------------------------------------------------------------------------
    // Storage. All static — one set of slots per instantiation, matching
    // the process-global nature of POSIX signal disposition.
    // -------------------------------------------------------------------------

    // Per-signal callback registries. Each registry owns its callbacks by
    // value via inplace_function's inline buffer — zero heap.
    static inline std::array<registry_type, signal_count> registries_{};

    // Per-signal enable flags — indexed by index_of<Signum>().
    static inline std::bitset<signal_count> enabled_{};

    // Publish/teardown gate. Set to true by install() with release
    // ordering after every prior setup step is complete; set to false by
    // uninstall() with release ordering before rewinding sigaction.
    // os_handler reads it with acquire ordering as its very first step.
    //
    // Requires std::atomic<bool> to be lock-free — which C++ guarantees
    // via is_always_lock_free on all mainstream targets, and which is the
    // de-facto condition under which std::atomic operations are
    // async-signal-safe. If a platform ever fails this static_assert, the
    // gate would have to be reimplemented in terms of
    // volatile std::sig_atomic_t + std::atomic_signal_fence, losing the
    // cross-thread acquire/release contract.
    static_assert(std::atomic<bool>::is_always_lock_free,
                  "inplace_signal_event: std::atomic<bool> must be lock-free for "
                  "async-signal-safe release/acquire gating");
    static inline std::atomic<bool> ready_{false};

    // Compile-time list of managed signal numbers as raw ints — the form
    // consumed by <csignal> (sigaction, SIG_DFL, ...) and by the async-
    // signal handler's runtime lookup. Populated by converting each
    // enumerator in the Signals... pack via to_signum().
    static constexpr int signal_list_[signal_count] = { to_signum(Signals)... };
};

// Out-of-class definition for the constexpr signal_list_ array — required
// pre-C++17 for ODR-use; harmless in C++17 where the in-class initializer
// is already implicitly inline. Kept explicit for maximum toolchain
// portability on older embedded compilers.
template <std::size_t MaxCallbackPerSignal, std::size_t CallbackStorageSize, std::size_t CallbackStorageAlignment, signal... Signals>
constexpr int inplace_signal_event<MaxCallbackPerSignal, CallbackStorageSize, CallbackStorageAlignment, Signals...>::signal_list_[];

} // namespace events
} // namespace castle
