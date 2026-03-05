/**
 * MIT License
 *
 * Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * ============================================================
 * GNSS Location Service – Diagnostics Module
 * ============================================================
 *
 * ARCHITECTURE OVERVIEW
 * ---------------------
 *
 *  ┌─────────────────────────────────────────────────────────────────────┐
 *  │                      GNSS Chip / HW Layer                           │
 *  │  (position fix, satellite count, signal strength, velocity, time)   │
 *  └──────────────────────────┬──────────────────────────────────────────┘
 *                             │ raw data stream (10 Hz)
 *                             ▼
 *  ┌─────────────────────────────────────────────────────────────────────┐
 *  │                    gnss_data_pipeline                               │
 *  │  Validates each incoming GNSS frame and fires typed Events          │
 *  └──────────────────────────┬──────────────────────────────────────────┘
 *                             │ IEvent (typed)
 *                             ▼
 *  ┌─────────────────────────────────────────────────────────────────────┐
 *  │                    event_bus  (Observer / Pub-Sub)                  │
 *  │  Routes events to all registered IEventListener subscribers         │
 *  └──────┬───────────────────┬───────────────────────────┬─────────────┘
 *         │                   │                           │
 *         ▼                   ▼                           ▼
 *  ┌─────────────┐  ┌──────────────────────┐  ┌──────────────────────┐
 *  │ dtc_monitor │  │ signal_quality_mon.  │  │  fix_continuity_mon. │
 *  │             │  │                      │  │                      │
 *  │ Counts fail │  │ Sliding-window avg   │  │ Detects fix-loss     │
 *  │ events into │  │ of CNR / PDOP        │  │ and gap duration     │
 *  │ debounce    │  │                      │  │                      │
 *  │ counters    │  └──────────┬───────────┘  └──────────┬───────────┘
 *  └──────┬──────┘             │                         │
 *         │              DTC fires when thresholds crossed
 *         ▼                   ▼                         ▼
 *  ┌─────────────────────────────────────────────────────────────────────┐
 *  │                    dtc_manager  (Registry + State Machine)          │
 *  │                                                                     │
 *  │  For each DTC id → DiagFaultState: PASSED / PENDING / FAILED       │
 *  │  Threshold-based debounce: fail_count / pass_count windows          │
 *  │  Stores freeze-frame snapshot at first failure                      │
 *  └──────────────────────────┬──────────────────────────────────────────┘
 *                             │ DTC confirmed active / healed
 *                             ▼
 *  ┌─────────────────────────────────────────────────────────────────────┐
 *  │                 reaction_dispatcher  (Chain-of-Responsibility)      │
 *  │                                                                     │
 *  │  DTC → ordered chain of IReactionHandler                            │
 *  │   1. log_handler         – structured log entry                     │
 *  │   2. notify_handler      – IReactionObserver callbacks              │
 *  │   3. recovery_handler    – request chip reset / fallback mode       │
 *  │   4. dtc_storage_handler – persist to NVM / UDS DTC storage         │
 *  └─────────────────────────────────────────────────────────────────────┘
 *
 * DESIGN PATTERNS USED
 * --------------------
 *  - Observer / Pub-Sub  : event_bus decouples data pipeline from monitors
 *  - State Machine       : dtc_fault_state per DTC (PASSED→PENDING→FAILED)
 *  - Chain of Responsibility : reaction_dispatcher chains handlers
 *  - Strategy            : IMonitorStrategy – plug-in different algorithms
 *  - Template Method     : base_monitor defines the skeleton; subclasses fill steps
 *  - Command             : IReactionHandler encapsulates a reaction as an object
 *  - Registry / Factory  : dtc_manager owns DTC definitions and instances
 *
 * DATA STRUCTURES
 * ---------------
 *  - std::unordered_map<dtc_id, dtc_context>  – O(1) DTC lookup
 *  - std::deque (sliding window)              – signal quality averaging
 *  - std::priority_queue                      – reaction priority ordering
 *  - std::vector<IEventListener*>             – subscriber list per event type
 */

#ifndef GNSS_DIAGNOSTICS_H
#define GNSS_DIAGNOSTICS_H

#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <map>
#include <functional>
#include <memory>
#include <chrono>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iomanip>

namespace gnss_diag
{

// ============================================================
//  SECTION 1: Types and Enumerations
// ============================================================

/** Milliseconds since epoch */
using timestamp_ms = uint64_t;

/** Unique identifier for a Diagnostic Trouble Code */
using dtc_id = uint32_t;

/** DTC severity as defined in ISO 14229 / AUTOSAR UDS */
enum class dtc_severity : uint8_t
{
    INFO        = 0x00,  ///< Informational, no service required
    LOW         = 0x01,  ///< Degraded performance
    MEDIUM      = 0x02,  ///< Loss of function
    HIGH        = 0x03,  ///< Safety relevant
    CRITICAL    = 0x04   ///< Immediate service required
};

/** Per-DTC state machine states */
enum class dtc_fault_state : uint8_t
{
    PASSED  = 0,  ///< No fault detected
    PENDING = 1,  ///< Fault detected but not yet debounced
    FAILED  = 2,  ///< Fault confirmed after debounce threshold
    HEALED  = 3   ///< Previously failed, now passing
};

inline const char* to_string(dtc_fault_state s)
{
    switch (s)
    {
        case dtc_fault_state::PASSED:  return "PASSED";
        case dtc_fault_state::PENDING: return "PENDING";
        case dtc_fault_state::FAILED:  return "FAILED";
        case dtc_fault_state::HEALED:  return "HEALED";
    }
    return "UNKNOWN";
}

inline const char* to_string(dtc_severity s)
{
    switch (s)
    {
        case dtc_severity::INFO:     return "INFO";
        case dtc_severity::LOW:      return "LOW";
        case dtc_severity::MEDIUM:   return "MEDIUM";
        case dtc_severity::HIGH:     return "HIGH";
        case dtc_severity::CRITICAL: return "CRITICAL";
    }
    return "UNKNOWN";
}

// ============================================================
//  SECTION 2: GNSS Data Frame
// ============================================================

/**
 * Raw frame produced by GNSS chip at each epoch (typically 100 ms / 10 Hz)
 *
 * Fields map roughly to NMEA/UBX outputs:
 *  - fix_valid    : GGA quality indicator > 0
 *  - satellite_count : GSV total sats in view
 *  - used_sats    : GGA satellites used in solution
 *  - hdop/vdop/pdop : GGA/GSA dilution of precision
 *  - latitude/longitude/altitude : GGA
 *  - speed_kmh    : VTG ground speed
 *  - heading_deg  : VTG true course
 *  - cnr_avg_db   : average carrier-to-noise ratio from GSV
 *  - age_of_diff  : age of differential corrections (seconds)
 */
struct gnss_frame
{
    timestamp_ms  timestamp       = 0;
    bool          fix_valid       = false;
    uint8_t       fix_type        = 0;     ///< 0=none,1=GPS,2=DGPS,3=PPS,4=RTK,5=FloatRTK,6=Est,7=Manual,8=Sim
    uint8_t       satellite_count = 0;     ///< total sats in view
    uint8_t       used_sats       = 0;     ///< sats used in fix
    float         hdop            = 99.f;
    float         vdop            = 99.f;
    float         pdop            = 99.f;
    double        latitude        = 0.0;
    double        longitude       = 0.0;
    float         altitude_m      = 0.f;
    float         speed_kmh       = 0.f;
    float         heading_deg     = 0.f;
    float         cnr_avg_db      = 0.f;   ///< average CNR dBHz
    float         age_of_diff_s   = 0.f;
    bool          leap_second_valid = false;
};

// ============================================================
//  SECTION 3: Freeze-Frame Snapshot (stored on first failure)
// ============================================================

struct freeze_frame
{
    timestamp_ms snapshot_time = 0;
    gnss_frame   frame;
    std::string  description;

    std::string to_string() const
    {
        std::ostringstream oss;
        oss << "[FreezeFrame @ " << snapshot_time << " ms] "
            << description
            << " | fix=" << (frame.fix_valid ? "Y" : "N")
            << " sats=" << (int)frame.used_sats
            << "/" << (int)frame.satellite_count
            << " HDOP=" << std::fixed << std::setprecision(1) << frame.hdop
            << " CNR=" << frame.cnr_avg_db << "dBHz"
            << " PDOP=" << frame.pdop;
        return oss.str();
    }
};

// ============================================================
//  SECTION 4: Event System (Observer / Pub-Sub)
// ============================================================

/** All event types the pipeline can emit */
enum class event_type : uint16_t
{
    // --- Fix quality events ---
    FIX_ACQUIRED           = 0x0001,
    FIX_LOST               = 0x0002,
    FIX_DEGRADED           = 0x0003,   ///< fix valid but DOP high

    // --- Signal events ---
    SIGNAL_WEAK            = 0x0010,   ///< CNR below threshold
    SIGNAL_RECOVERED       = 0x0011,

    // --- Satellite events ---
    SAT_COUNT_LOW          = 0x0020,
    SAT_COUNT_RECOVERED    = 0x0021,

    // --- Time events ---
    CLOCK_JUMP_DETECTED    = 0x0030,
    LEAP_SECOND_INVALID    = 0x0031,

    // --- Velocity / heading ---
    VELOCITY_SPIKE         = 0x0040,   ///< unrealistic speed jump
    HEADING_SPIKE          = 0x0041,

    // --- Position plausibility ---
    POSITION_JUMP          = 0x0050,   ///< position distance jump > threshold
    ALTITUDE_OUT_OF_RANGE  = 0x0051,

    // --- Chip / protocol ---
    NO_DATA_TIMEOUT        = 0x0060,   ///< no frame received within timeout window
    CHECKSUM_ERROR         = 0x0061,
    CHIP_RESET             = 0x0070,

    // --- Catchall ---
    CUSTOM                 = 0xFFFF
};

/** Base event – carries the triggering frame and a timestamp */
struct IEvent
{
    event_type    type;
    timestamp_ms  timestamp;
    gnss_frame    frame;       ///< snapshot of frame that caused event
    std::string   detail;      ///< human-readable description

    IEvent(event_type t, timestamp_ms ts, const gnss_frame& f, const std::string& d = "")
        : type(t), timestamp(ts), frame(f), detail(d)
    {}

    virtual ~IEvent() = default;
};

/** Listener interface – implement this to subscribe to events */
class IEventListener
{
public:
    virtual ~IEventListener() = default;
    virtual void on_event(const IEvent& event) = 0;
};

/**
 * event_bus
 *
 * Central message broker using Observer pattern.
 * Producers post events; consumers subscribe per event_type.
 *
 * Data structure: std::unordered_map<event_type, vector<IEventListener*>>
 * O(1) lookup, O(n) dispatch where n = subscriber count (typically small).
 */
class event_bus
{
public:
    void subscribe(event_type type, IEventListener* listener)
    {
        if (listener)
        {
            m_subscribers[type].push_back(listener);
        }
    }

    void subscribe_all(IEventListener* listener)
    {
        if (listener)
        {
            m_catch_all.push_back(listener);
        }
    }

    void unsubscribe(event_type type, IEventListener* listener)
    {
        auto it = m_subscribers.find(type);
        if (it != m_subscribers.end())
        {
            auto& vec = it->second;
            vec.erase(std::remove(vec.begin(), vec.end(), listener), vec.end());
        }
    }

    void post(const IEvent& event) const
    {
        // Dispatch to type-specific subscribers
        auto it = m_subscribers.find(event.type);
        if (it != m_subscribers.end())
        {
            for (auto* l : it->second)
            {
                if (l) l->on_event(event);
            }
        }

        // Dispatch to catch-all listeners
        for (auto* l : m_catch_all)
        {
            if (l) l->on_event(event);
        }
    }

private:
    std::unordered_map<event_type, std::vector<IEventListener*>> m_subscribers;
    std::vector<IEventListener*> m_catch_all;
};

// ============================================================
//  SECTION 5: DTC Definition and Context
// ============================================================

/**
 * dtc_definition
 *
 * Static metadata for a single Diagnostic Trouble Code.
 * Inspired by OBD-II / UDS (ISO 14229) conventions.
 */
struct dtc_definition
{
    dtc_id        id;
    std::string   name;
    std::string   description;
    dtc_severity  severity;
    uint16_t      fail_threshold;   ///< consecutive failures before FAILED
    uint16_t      pass_threshold;   ///< consecutive passes before HEALED/PASSED
};

/**
 * dtc_context
 *
 * Runtime state for one DTC instance.
 * Implements a counter-based debounce state machine:
 *
 *   PASSED ──[fail_count reaches threshold]──► PENDING
 *   PENDING──[fail_count reaches threshold]──► FAILED
 *   FAILED ──[pass_count reaches threshold]──► HEALED
 *   HEALED ──[one more pass cycle]──────────► PASSED
 *
 * Data structure: plain counters + std::optional freeze frame.
 */
struct dtc_context
{
    const dtc_definition* def = nullptr;

    dtc_fault_state state       = dtc_fault_state::PASSED;
    uint16_t        fail_count  = 0;
    uint16_t        pass_count  = 0;
    uint32_t        trip_count  = 0;   ///< total times transitioned to FAILED
    timestamp_ms    last_fail_ts = 0;
    timestamp_ms    last_pass_ts = 0;

    freeze_frame    first_failure_frame; ///< snapshot stored at first FAILED transition
    bool            freeze_stored = false;

    /**
     * Feed a failure detection result.
     * Returns true if the DTC state just transitioned (caller should react).
     */
    bool on_fail(const gnss_frame& frame)
    {
        pass_count = 0;
        last_fail_ts = frame.timestamp;

        if (state == dtc_fault_state::FAILED)
        {
            return false; // already confirmed, no new transition
        }

        ++fail_count;
        if (fail_count >= def->fail_threshold)
        {
            dtc_fault_state prev = state;
            state = dtc_fault_state::FAILED;
            fail_count = def->fail_threshold; // cap

            if (!freeze_stored)
            {
                first_failure_frame.snapshot_time = frame.timestamp;
                first_failure_frame.frame         = frame;
                first_failure_frame.description   = def->name;
                freeze_stored                     = true;
            }

            if (prev != dtc_fault_state::FAILED)
            {
                ++trip_count;
                return true;
            }
        }
        else if (state == dtc_fault_state::PASSED || state == dtc_fault_state::HEALED)
        {
            state = dtc_fault_state::PENDING;
            return true;
        }

        return false;
    }

    /**
     * Feed a pass (no-fault) detection result.
     * Returns true if the DTC state just transitioned.
     */
    bool on_pass(const gnss_frame& frame)
    {
        fail_count = 0;
        last_pass_ts = frame.timestamp;

        if (state == dtc_fault_state::PASSED)
        {
            return false;
        }

        ++pass_count;
        if (pass_count >= def->pass_threshold)
        {
            pass_count = def->pass_threshold; // cap

            if (state == dtc_fault_state::HEALED)
            {
                state = dtc_fault_state::PASSED;
                freeze_stored = false; // allow new capture next time
                return true;
            }
            else if (state == dtc_fault_state::FAILED)
            {
                state = dtc_fault_state::HEALED;
                return true;
            }
            else if (state == dtc_fault_state::PENDING)
            {
                state = dtc_fault_state::PASSED;
                fail_count = 0;
                return true;
            }
        }

        return false;
    }

    bool is_active() const
    {
        return state == dtc_fault_state::FAILED || state == dtc_fault_state::PENDING;
    }
};

// ============================================================
//  SECTION 6: DTC Manager (Registry + State Machine Controller)
// ============================================================

/**
 * Callback invoked when a DTC transitions state.
 * Arguments: dtc_context (current state), is_set (true=fault, false=cleared)
 */
using dtc_state_callback = std::function<void(const dtc_context&, bool is_set)>;

/**
 * dtc_manager
 *
 * Owns all DTC definitions and their runtime contexts.
 * Accepts raw monitor verdicts (pass/fail) per DTC id and drives
 * the per-DTC state machine.
 *
 * Data structure: std::unordered_map<dtc_id, dtc_context> for O(1) access.
 */
class dtc_manager
{
public:
    void register_dtc(const dtc_definition& def)
    {
        m_defs[def.id]          = def;
        m_contexts[def.id].def  = &m_defs[def.id];
    }

    /**
     * Report a failure event for a specific DTC.
     * If the DTC transitions, the state_callback is invoked.
     */
    void report_fail(dtc_id id, const gnss_frame& frame)
    {
        auto it = m_contexts.find(id);
        if (it == m_contexts.end()) return;

        bool transitioned = it->second.on_fail(frame);
        if (transitioned && m_callback)
        {
            m_callback(it->second, true);
        }
    }

    /**
     * Report a pass (recovery) event for a specific DTC.
     */
    void report_pass(dtc_id id, const gnss_frame& frame)
    {
        auto it = m_contexts.find(id);
        if (it == m_contexts.end()) return;

        bool transitioned = it->second.on_pass(frame);
        if (transitioned && m_callback)
        {
            m_callback(it->second, false);
        }
    }

    void set_state_callback(dtc_state_callback cb)
    {
        m_callback = std::move(cb);
    }

    const dtc_context* get_context(dtc_id id) const
    {
        auto it = m_contexts.find(id);
        return (it != m_contexts.end()) ? &it->second : nullptr;
    }

    /** Returns all DTCs currently in FAILED or PENDING state */
    std::vector<const dtc_context*> get_active_dtcs() const
    {
        std::vector<const dtc_context*> result;
        for (const auto& kv : m_contexts)
        {
            if (kv.second.is_active())
                result.push_back(&kv.second);
        }
        return result;
    }

    /** Dump all DTC states to stdout */
    void print_status() const
    {
        printf("┌─────────────────────────────────────────────────────────\n");
        printf("│  DTC Status Report\n");
        printf("├──────────┬──────────────────────────┬──────────┬───────\n");
        printf("│ DTC ID   │ Name                     │ State    │ Trips \n");
        printf("├──────────┼──────────────────────────┼──────────┼───────\n");

        // Use ordered map for deterministic printout
        std::map<dtc_id, const dtc_context*> ordered;
        for (const auto& kv : m_contexts)
            ordered[kv.first] = &kv.second;

        for (const auto& kv : ordered)
        {
            const dtc_context& ctx = *kv.second;
            printf("│ 0x%06X │ %-24s │ %-8s │ %5u \n",
                   ctx.def->id,
                   ctx.def->name.c_str(),
                   to_string(ctx.state),
                   ctx.trip_count);
        }
        printf("└──────────┴──────────────────────────┴──────────┴───────\n");
    }

private:
    std::unordered_map<dtc_id, dtc_definition> m_defs;
    std::unordered_map<dtc_id, dtc_context>    m_contexts;
    dtc_state_callback                          m_callback;
};

// ============================================================
//  SECTION 7: Monitor Strategy (Template Method + Strategy)
// ============================================================

/**
 * IMonitorStrategy
 *
 * A pluggable algorithm that decides, for one aspect of GNSS quality,
 * whether to report pass or fail to the dtc_manager.
 *
 * Template Method pattern: evaluate() is the skeleton;
 *   - check_condition() is the strategy hook
 *   - get_dtc_id()      returns which DTC this strategy targets
 */
class IMonitorStrategy
{
public:
    virtual ~IMonitorStrategy() = default;

    /**
     * Called for each incoming GNSS frame.
     * Internally calls check_condition and reports to dtc_manager.
     */
    void evaluate(const gnss_frame& frame, dtc_manager& mgr)
    {
        if (check_condition(frame))
            mgr.report_fail(get_dtc_id(), frame);
        else
            mgr.report_pass(get_dtc_id(), frame);
    }

protected:
    /** Returns true when a fault condition is detected */
    virtual bool check_condition(const gnss_frame& frame) = 0;

    /** DTC id this strategy reports to */
    virtual dtc_id get_dtc_id() const = 0;
};

// ============================================================
//  SECTION 8: Reaction System (Chain of Responsibility + Command)
// ============================================================

/**
 * IReactionHandler
 *
 * One node in the chain-of-responsibility for DTC reactions.
 * Each handler decides whether to handle the event itself and
 * whether to pass it along to the next handler.
 */
class IReactionHandler
{
public:
    virtual ~IReactionHandler() = default;

    void set_next(std::shared_ptr<IReactionHandler> next)
    {
        m_next = std::move(next);
    }

    virtual void handle(const dtc_context& ctx, bool is_set)
    {
        // Default: always pass to next handler
        if (m_next)
            m_next->handle(ctx, is_set);
    }

protected:
    std::shared_ptr<IReactionHandler> m_next;
};

/**
 * reaction_dispatcher
 *
 * Builds and owns the chain.
 * Dispatches DTC state changes into the head of the chain.
 */
class reaction_dispatcher
{
public:
    /**
     * Add a handler to the tail of the chain.
     * Order matters: handlers execute in insertion order.
     */
    void add_handler(std::shared_ptr<IReactionHandler> handler)
    {
        if (!m_head)
        {
            m_head = handler;
            m_tail = handler;
        }
        else
        {
            m_tail->set_next(handler);
            m_tail = handler;
        }
    }

    void dispatch(const dtc_context& ctx, bool is_set) const
    {
        if (m_head)
            m_head->handle(ctx, is_set);
    }

private:
    std::shared_ptr<IReactionHandler> m_head;
    std::shared_ptr<IReactionHandler> m_tail;
};

// ============================================================
//  SECTION 9: Built-in Reaction Handlers
// ============================================================

/** Prints a structured log line to stdout */
class log_handler : public IReactionHandler
{
public:
    void handle(const dtc_context& ctx, bool is_set) override
    {
        printf("[DIAG] DTC 0x%06X %-24s → %-8s  sev=%-8s  trips=%u\n",
               ctx.def->id,
               ctx.def->name.c_str(),
               to_string(ctx.state),
               to_string(ctx.def->severity),
               ctx.trip_count);

        if (is_set && ctx.freeze_stored)
        {
            printf("       %s\n", ctx.first_failure_frame.to_string().c_str());
        }

        IReactionHandler::handle(ctx, is_set);
    }
};

/**
 * notify_handler
 *
 * Calls registered IReactionObserver callbacks.
 * Allows upper-layer software (e.g., HMI, ADAS) to react without coupling
 * to the internals of the diagnostic module.
 */
class IReactionObserver
{
public:
    virtual ~IReactionObserver() = default;
    virtual void on_dtc_set(const dtc_context& ctx)     = 0;
    virtual void on_dtc_cleared(const dtc_context& ctx) = 0;
};

class notify_handler : public IReactionHandler
{
public:
    void add_observer(IReactionObserver* obs)
    {
        if (obs)
            m_observers.push_back(obs);
    }

    void handle(const dtc_context& ctx, bool is_set) override
    {
        for (auto* obs : m_observers)
        {
            if (is_set)
                obs->on_dtc_set(ctx);
            else
                obs->on_dtc_cleared(ctx);
        }
        IReactionHandler::handle(ctx, is_set);
    }

private:
    std::vector<IReactionObserver*> m_observers;
};

/**
 * recovery_handler
 *
 * Executes recovery actions based on DTC severity.
 * Pluggable via std::function to avoid hard-coding HW access.
 */
class recovery_handler : public IReactionHandler
{
public:
    using RecoveryAction = std::function<void(const dtc_context&)>;

    void set_chip_reset_action(RecoveryAction action)
    {
        m_chip_reset = std::move(action);
    }

    void set_fallback_action(RecoveryAction action)
    {
        m_fallback = std::move(action);
    }

    void handle(const dtc_context& ctx, bool is_set) override
    {
        if (!is_set)
        {
            // DTC clearing – no recovery needed
            IReactionHandler::handle(ctx, is_set);
            return;
        }

        switch (ctx.def->severity)
        {
            case dtc_severity::CRITICAL:
            case dtc_severity::HIGH:
                if (m_chip_reset)
                {
                    printf("[RECOVERY] Requesting GNSS chip reset for DTC 0x%06X\n", ctx.def->id);
                    m_chip_reset(ctx);
                }
                break;

            case dtc_severity::MEDIUM:
                if (m_fallback)
                {
                    printf("[RECOVERY] Activating fallback mode for DTC 0x%06X\n", ctx.def->id);
                    m_fallback(ctx);
                }
                break;

            default:
                break;
        }

        IReactionHandler::handle(ctx, is_set);
    }

private:
    RecoveryAction m_chip_reset;
    RecoveryAction m_fallback;
};

/**
 * dtc_storage_handler
 *
 * Persists confirmed DTCs to a simple in-memory NVM simulation.
 * In production this would write to EEPROM/Flash via a PAL layer.
 *
 * Data structure: std::vector<dtc_id> representing NVM storage.
 */
class dtc_storage_handler : public IReactionHandler
{
public:
    void handle(const dtc_context& ctx, bool is_set) override
    {
        if (is_set && ctx.state == dtc_fault_state::FAILED)
        {
            auto it = std::find(m_nvm.begin(), m_nvm.end(), ctx.def->id);
            if (it == m_nvm.end())
            {
                m_nvm.push_back(ctx.def->id);
                printf("[NVM] Stored DTC 0x%06X (%s)\n", ctx.def->id, ctx.def->name.c_str());
            }
        }
        else if (!is_set && ctx.state == dtc_fault_state::PASSED)
        {
            m_nvm.erase(std::remove(m_nvm.begin(), m_nvm.end(), ctx.def->id), m_nvm.end());
            printf("[NVM] Cleared DTC 0x%06X (%s)\n", ctx.def->id, ctx.def->name.c_str());
        }

        IReactionHandler::handle(ctx, is_set);
    }

    const std::vector<dtc_id>& get_stored_dtcs() const { return m_nvm; }

private:
    std::vector<dtc_id> m_nvm;
};

// ============================================================
//  SECTION 10: Sliding-Window Signal Quality Monitor
// ============================================================

/**
 * sliding_window_avg
 *
 * Generic fixed-size sliding window for floating-point metrics.
 * Data structure: std::deque<float> – O(1) push/pop, O(n) average.
 */
class sliding_window_avg
{
public:
    explicit sliding_window_avg(size_t window_size)
        : m_window_size(window_size), m_sum(0.0)
    {}

    void push(float value)
    {
        m_buf.push_back(value);
        m_sum += value;

        if (m_buf.size() > m_window_size)
        {
            m_sum -= m_buf.front();
            m_buf.pop_front();
        }
    }

    float average() const
    {
        if (m_buf.empty()) return 0.f;
        return static_cast<float>(m_sum / m_buf.size());
    }

    size_t count() const { return m_buf.size(); }
    bool   full()  const { return m_buf.size() >= m_window_size; }

private:
    size_t       m_window_size;
    double       m_sum;
    std::deque<float> m_buf;
};

} // namespace gnss_diag

#endif // GNSS_DIAGNOSTICS_H
