#pragma once

#include <csignal>

namespace castle
{
namespace events
{

// -----------------------------------------------------------------------------
// castle::events::signal
// -----------------------------------------------------------------------------
// Strongly-typed enumeration of the POSIX signals that signal_event can
// manage. The underlying values are the platform's SIG* macros so the enum
// converts losslessly to the `int` expected by <csignal> APIs (sigaction,
// raise, kill, ...).
//
// Rationale:
//   - Type safety: signal_event<..., signal::sigint, signal::sigsegv> refuses
//     to compile if a caller accidentally passes an unrelated integer.
//   - Self-documenting template parameter lists — you read the intent at the
//     call site instead of a bare int literal.
//   - Portable enumerator names (all lower-case, no SIG prefix) that follow
//     the castle naming style, while still mapping 1:1 to the POSIX macros.
//
// Only the signals that are meaningful to catch in user space are exposed
// here. SIGKILL and SIGSTOP are intentionally omitted because the kernel
// forbids installing a handler for them; adding them to the enum would
// invite install() failures at runtime.
// -----------------------------------------------------------------------------
enum class signal : int
{
    sighup    = SIGHUP,     // controlling terminal closed / config reload convention
    sigint    = SIGINT,     // interactive attention (Ctrl-C)
    sigquit   = SIGQUIT,    // interactive termination + core dump (Ctrl-\)
    sigill    = SIGILL,     // illegal instruction
    sigtrap   = SIGTRAP,    // trace / breakpoint trap
    sigabrt   = SIGABRT,    // abort()
    sigbus    = SIGBUS,     // bus error / misaligned access
    sigfpe    = SIGFPE,     // floating-point / integer arithmetic error
    sigusr1   = SIGUSR1,    // user-defined 1
    sigsegv   = SIGSEGV,    // invalid memory reference
    sigusr2   = SIGUSR2,    // user-defined 2
    sigpipe   = SIGPIPE,    // write to pipe with no reader
    sigalrm   = SIGALRM,    // alarm(2) timer
    sigterm   = SIGTERM,    // termination request
    sigchld   = SIGCHLD,    // child process state change
    sigcont   = SIGCONT,    // continue if stopped
    sigtstp   = SIGTSTP,    // interactive stop (Ctrl-Z)
    sigttin   = SIGTTIN,    // background read from tty
    sigttou   = SIGTTOU,    // background write to tty
    sigurg    = SIGURG,     // out-of-band data on socket
    sigxcpu   = SIGXCPU,    // CPU time limit exceeded
    sigxfsz   = SIGXFSZ,    // file size limit exceeded
    sigvtalrm = SIGVTALRM,  // virtual timer expired
    sigprof   = SIGPROF,    // profiling timer expired
    sigsys    = SIGSYS      // bad system call
};

// Utility: convert to the plain `int` expected by <csignal> APIs. Kept as a
// free constexpr helper so both public code and signal_event's internals can
// use the same conversion path (no ad-hoc static_cast<int> sprinkled around).
constexpr int to_signum(signal s) noexcept
{
    return static_cast<int>(s);
}

} // namespace events
} // namespace castle
