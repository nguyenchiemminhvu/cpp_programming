#pragma once
// =============================================================================
// BAD DESIGN: Classic OOD Observer + std::function
//
// Problems demonstrated in this header:
//   1. Virtual dispatch + vtable overhead for every concrete listener.
//   2. std::vector<T*> => ambiguous ownership (who deletes the listeners?).
//   3. std::function Small Object Optimization (SOO) trap: a lambda closure
//      larger than the internal buffer (~16-32 bytes) SILENTLY heap-allocates,
//      with no compile-time warning.
//   4. std::vector::push_back may trigger an O(n) reallocation, which is
//      catastrophic inside an ISR or a real-time CAN receive task.
//   5. std::function's constructor may throw std::bad_alloc, which is undefined
//      behaviour in a -fno-exceptions embedded build.
// =============================================================================

#include <vector>
#include <functional>
#include <iostream>
#include <string>

// -----------------------------------------------------------------------------
// PROBLEM 1: A pure-virtual base forces a separate heap allocation for every
// concrete listener and pays a vtable-pointer cost (8 bytes on 64-bit targets).
// -----------------------------------------------------------------------------
class can_signal_listener
{
public:
    virtual ~can_signal_listener() = default;
    virtual void on_signal_received(int signal_id, double value) = 0;
};

class can_dispatcher
{
public:
    void register_listener(can_signal_listener* listener)
    {
        // PROBLEM 4: push_back may reallocate the whole vector once the current
        // capacity is exhausted. In a real-time task this produces a
        // non-deterministic latency spike.
        listeners_.push_back(listener);
    }

    template <typename F>
    void register_callback(F&& callback)
    {
        // PROBLEM 5: the std::function constructor can throw std::bad_alloc.
        // In an embedded build compiled with -fno-exceptions this becomes UB.
        alternative_callbacks_.emplace_back(std::forward<F>(callback));
    }

    void dispatch(int id, double value)
    {
        for (auto* listener : listeners_)
        {
            // The null-check below only masks the ambiguous-ownership smell.
            if (listener)
            {
                listener->on_signal_received(id, value);
            }
        }

        for (auto& callback : alternative_callbacks_)
        {
            callback(id, value);
        }
    }

private:
    // PROBLEM 2: raw-pointer vector => ownership is unclear. If a listener is
    // destroyed before the dispatcher, this becomes a dangling pointer => UB.
    std::vector<can_signal_listener*> listeners_;

    // PROBLEM 3: std::function SOO trap. std::function holds an internal buffer
    // of typically 16-32 bytes. Any callable capturing more than that buffer
    // silently allocates on the heap; there is no way to know at compile time.
    std::vector<std::function<void(int, double)>> alternative_callbacks_;
};

// -----------------------------------------------------------------------------
// DEMO: the SOO trap in action.
// heavy_context is ~68 bytes, far larger than std::function's SOO buffer, so
// wrapping it in a std::function GUARANTEES a heap allocation every time.
// -----------------------------------------------------------------------------
struct heavy_context
{
    std::string ecu_name;             // ~32 bytes (typical SSO layout)
    double      calibration_table[4]; // 32 bytes
    int         node_id;              //  4 bytes
    // Total ~68 bytes >> SOO buffer => guaranteed heap allocation.
};

inline void demonstrate_soo_trap()
{
    heavy_context ctx{"EngineControlUnit", {1.0, 2.0, 3.0, 4.0}, 42};

    can_dispatcher dispatcher;

    // Looks innocent, but this secretly allocates on the heap on every call.
    dispatcher.register_callback([ctx](int id, double value)
    {
        std::cout << ctx.ecu_name << ": signal " << id << " = " << value << "\n";
    });
}
