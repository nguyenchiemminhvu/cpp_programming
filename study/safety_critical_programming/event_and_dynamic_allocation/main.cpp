// =============================================================================
// Lesson 1 - Event-Driven System & Dynamic Callback Allocation
//
// This demo contrasts the bad, heap-based observer against the preferred,
// zero-allocation dispatcher. Build and run it to observe the deterministic,
// allocation-free dispatch path in action.
// =============================================================================

#include "bad_design.h"
#include "preferred_design.h"

#include <cstdio>

// -----------------------------------------------------------------------------
// A simulated ECU component that carries state and behaves as a functor, so it
// is directly compatible with inplace_function.
// -----------------------------------------------------------------------------
struct throttle_controller
{
    double        current_throttle = 0.0;
    std::uint8_t  node_id          = 0;

    void operator()(const can_signal& sig) noexcept
    {
        if (sig.signal_id == 0x1A0) // throttle position signal id
        {
            current_throttle = sig.value;
            std::printf("[Node %u] Throttle updated: %.2f%%\n",
                        node_id, current_throttle);
        }
    }
};

// A plain free function also works as a callback.
void engine_speed_handler(const can_signal& sig)
{
    std::printf("[EngineSpeed] Signal 0x%X = %.1f RPM at t=%u ms\n",
                sig.signal_id, sig.value, sig.timestamp_ms);
}

int main()
{
    using dispatcher_t = non_allocating_can_dispatcher<8, 48>;
    dispatcher_t dispatcher;

    // -- Registration 1: free function -> function pointer (no capture) -------
    {
        const auto err = dispatcher.register_callback(
            dispatcher_t::callback{engine_speed_handler},
            0x0C0); // only accept signal id 0x0C0
        std::printf("Register engine_speed_handler: %s\n",
                    err == dispatcher_error::ok ? "OK" : "FAILED");
    }

    // -- Registration 2: stateful functor (throttle_controller) ---------------
    {
        throttle_controller throttle{0.0, 1};
        const auto err = dispatcher.register_callback(
            dispatcher_t::callback{throttle},
            0x1A0); // only accept the throttle signal
        std::printf("Register throttle_controller: %s\n",
                    err == dispatcher_error::ok ? "OK" : "FAILED");
    }

    // -- Registration 3: lambda with a small capture (compile-time safe) ------
    {
        const std::uint8_t bus_id = 0;
        const auto err = dispatcher.register_callback(
            [bus_id](const can_signal& sig) noexcept
            {
                std::printf("[Bus %u] Signal 0x%X received\n",
                            bus_id, sig.signal_id);
            });
        std::printf("Register lambda: %s\n",
                    err == dispatcher_error::ok ? "OK" : "FAILED");
    }

    // -- Simulate CAN dispatch ------------------------------------------------
    std::printf("\n--- Dispatching signals ---\n");
    dispatcher.dispatch({0x0C0, 3500.0, 0, 1000}); // engine speed
    dispatcher.dispatch({0x1A0, 75.5, 0, 1001});   // throttle position
    dispatcher.dispatch({0x200, 12.6, 0, 1002});   // battery voltage (no listener)

    // -- Compile-time safety demo (uncomment to see the build fail) -----------
    /*
    struct too_big
    {
        char data[100]; // 100 > 48 bytes of storage
        void operator()(const can_signal&) noexcept {}
    };
    dispatcher.register_callback(dispatcher_t::callback{too_big{}});
    // error: "Callable too large for inplace_function storage."
    // The failure appears at COMPILE TIME, not as a runtime crash.
    */

    return 0;
}