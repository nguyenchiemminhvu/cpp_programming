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
 * gnss_diagnostics_impl.h
 * ============================================================
 *
 * Concrete GNSS monitor strategies and the gnss_data_pipeline.
 *
 * Registered DTC Catalogue (matches ISO 14229 symptom-based naming):
 *
 *  DTC ID    | Name                       | Severity  | Description
 * -----------|----------------------------|-----------|-----------------------------------
 *  0x010001  | GNSS_NO_FIX                | HIGH      | No valid position fix
 *  0x010002  | GNSS_SAT_COUNT_LOW         | MEDIUM    | Fewer than min_sats used in fix
 *  0x010003  | GNSS_SIGNAL_WEAK           | LOW       | Average CNR below threshold
 *  0x010004  | GNSS_HDOP_HIGH             | MEDIUM    | Horizontal accuracy degraded
 *  0x010005  | GNSS_PDOP_HIGH             | MEDIUM    | 3D accuracy degraded
 *  0x010006  | GNSS_POSITION_JUMP         | HIGH      | Implausible position discontinuity
 *  0x010007  | GNSS_VELOCITY_SPIKE        | HIGH      | Implausible speed jump
 *  0x010008  | GNSS_NO_DATA_TIMEOUT       | CRITICAL  | No frame received within timeout
 *  0x010009  | GNSS_ALTITUDE_RANGE        | LOW       | Altitude outside plausible range
 *  0x01000A  | GNSS_CLOCK_JUMP            | MEDIUM    | GNSS time jumped unexpectedly
 *  0x01000B  | GNSS_LEAP_SECOND_INVALID   | INFO      | Leap second flag not valid
 */

#ifndef GNSS_DIAGNOSTICS_IMPL_H
#define GNSS_DIAGNOSTICS_IMPL_H

#pragma once

#include "gnss_diagnostics.h"

namespace gnss_diag
{

// ============================================================
//  SECTION 11: DTC ID Catalogue
// ============================================================

namespace dtc
{
    constexpr dtc_id NO_FIX               = 0x010001;
    constexpr dtc_id SAT_COUNT_LOW        = 0x010002;
    constexpr dtc_id SIGNAL_WEAK          = 0x010003;
    constexpr dtc_id HDOP_HIGH            = 0x010004;
    constexpr dtc_id PDOP_HIGH            = 0x010005;
    constexpr dtc_id POSITION_JUMP        = 0x010006;
    constexpr dtc_id VELOCITY_SPIKE       = 0x010007;
    constexpr dtc_id NO_DATA_TIMEOUT      = 0x010008;
    constexpr dtc_id ALTITUDE_RANGE       = 0x010009;
    constexpr dtc_id CLOCK_JUMP           = 0x01000A;
    constexpr dtc_id LEAP_SECOND_INVALID  = 0x01000B;
}

// ============================================================
//  SECTION 12: Thresholds (easily tunable)
// ============================================================

struct gnss_diag_config
{
    uint8_t  min_used_sats       = 4;       ///< below → SAT_COUNT_LOW
    float    min_cnr_db          = 25.0f;   ///< below → SIGNAL_WEAK
    float    max_hdop            = 2.5f;    ///< above → HDOP_HIGH
    float    max_pdop            = 4.0f;    ///< above → PDOP_HIGH
    double   max_position_jump_m = 50.0;    ///< above → POSITION_JUMP (per 100 ms)
    float    max_speed_kmh       = 250.0f;  ///< above → VELOCITY_SPIKE
    float    min_altitude_m      = -500.f;  ///< below → ALTITUDE_RANGE
    float    max_altitude_m      = 9000.f;  ///< above → ALTITUDE_RANGE
    float    max_clock_jump_ms   = 500.f;   ///< above → CLOCK_JUMP
    uint32_t no_data_timeout_ms  = 3000;    ///< above → NO_DATA_TIMEOUT
    size_t   cnr_window_size     = 10;      ///< sliding window for CNR average
};

// ============================================================
//  SECTION 13: Concrete Monitor Strategies
// ============================================================

/** GNSS_NO_FIX – no valid position fix */
class no_fix_strategy : public IMonitorStrategy
{
protected:
    bool check_condition(const gnss_frame& frame) override
    {
        return !frame.fix_valid;
    }
    dtc_id get_dtc_id() const override { return dtc::NO_FIX; }
};

/** GNSS_SAT_COUNT_LOW – too few satellites used in solution */
class sat_count_low_strategy : public IMonitorStrategy
{
public:
    explicit sat_count_low_strategy(uint8_t min_sats) : m_min_sats(min_sats) {}

protected:
    bool check_condition(const gnss_frame& frame) override
    {
        // Only meaningful when fix is valid
        return frame.fix_valid && (frame.used_sats < m_min_sats);
    }
    dtc_id get_dtc_id() const override { return dtc::SAT_COUNT_LOW; }

private:
    uint8_t m_min_sats;
};

/** GNSS_SIGNAL_WEAK – sliding-window averaged CNR below threshold */
class signal_weak_strategy : public IMonitorStrategy
{
public:
    signal_weak_strategy(float min_cnr_db, size_t window_size)
        : m_min_cnr(min_cnr_db), m_window(window_size)
    {}

protected:
    bool check_condition(const gnss_frame& frame) override
    {
        m_window.push(frame.cnr_avg_db);
        if (!m_window.full()) return false; // wait for window to fill
        return m_window.average() < m_min_cnr;
    }
    dtc_id get_dtc_id() const override { return dtc::SIGNAL_WEAK; }

private:
    float               m_min_cnr;
    sliding_window_avg  m_window;
};

/** GNSS_HDOP_HIGH – horizontal dilution of precision too large */
class hdop_high_strategy : public IMonitorStrategy
{
public:
    explicit hdop_high_strategy(float max_hdop) : m_max_hdop(max_hdop) {}

protected:
    bool check_condition(const gnss_frame& frame) override
    {
        return frame.fix_valid && (frame.hdop > m_max_hdop);
    }
    dtc_id get_dtc_id() const override { return dtc::HDOP_HIGH; }

private:
    float m_max_hdop;
};

/** GNSS_PDOP_HIGH – 3D dilution of precision too large */
class pdop_high_strategy : public IMonitorStrategy
{
public:
    explicit pdop_high_strategy(float max_pdop) : m_max_pdop(max_pdop) {}

protected:
    bool check_condition(const gnss_frame& frame) override
    {
        return frame.fix_valid && (frame.pdop > m_max_pdop);
    }
    dtc_id get_dtc_id() const override { return dtc::PDOP_HIGH; }

private:
    float m_max_pdop;
};

/**
 * GNSS_POSITION_JUMP
 *
 * Detects implausible position discontinuity using the Haversine formula.
 * A jump > max_jump_m per frame period is flagged.
 *
 * Haversine:  d = 2R·arcsin(√(sin²(Δφ/2) + cos(φ1)·cos(φ2)·sin²(Δλ/2)))
 */
class position_jump_strategy : public IMonitorStrategy
{
public:
    explicit position_jump_strategy(double max_jump_m)
        : m_max_jump_m(max_jump_m)
        , m_has_prev(false)
        , m_prev_lat(0.0)
        , m_prev_lon(0.0)
    {}

protected:
    bool check_condition(const gnss_frame& frame) override
    {
        if (!frame.fix_valid)
        {
            m_has_prev = false;
            return false;
        }

        if (!m_has_prev)
        {
            m_prev_lat = frame.latitude;
            m_prev_lon = frame.longitude;
            m_has_prev = true;
            return false;
        }

        double dist = haversine_m(m_prev_lat, m_prev_lon, frame.latitude, frame.longitude);
        m_prev_lat  = frame.latitude;
        m_prev_lon  = frame.longitude;

        return dist > m_max_jump_m;
    }

    dtc_id get_dtc_id() const override { return dtc::POSITION_JUMP; }

private:
    static constexpr double EARTH_RADIUS_M = 6371000.0;

    static double deg2rad(double deg) { return deg * M_PI / 180.0; }

    static double haversine_m(double lat1, double lon1, double lat2, double lon2)
    {
        double dlat = deg2rad(lat2 - lat1);
        double dlon = deg2rad(lon2 - lon1);
        double a = std::sin(dlat * 0.5) * std::sin(dlat * 0.5)
                 + std::cos(deg2rad(lat1)) * std::cos(deg2rad(lat2))
                 * std::sin(dlon * 0.5) * std::sin(dlon * 0.5);
        return EARTH_RADIUS_M * 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
    }

    double m_max_jump_m;
    bool   m_has_prev;
    double m_prev_lat;
    double m_prev_lon;
};

/** GNSS_VELOCITY_SPIKE – implausible speed */
class velocity_spike_strategy : public IMonitorStrategy
{
public:
    explicit velocity_spike_strategy(float max_speed_kmh) : m_max_speed(max_speed_kmh) {}

protected:
    bool check_condition(const gnss_frame& frame) override
    {
        return frame.fix_valid && (frame.speed_kmh > m_max_speed);
    }
    dtc_id get_dtc_id() const override { return dtc::VELOCITY_SPIKE; }

private:
    float m_max_speed;
};

/** GNSS_ALTITUDE_RANGE – altitude outside plausible earth surface range */
class altitude_range_strategy : public IMonitorStrategy
{
public:
    altitude_range_strategy(float min_alt, float max_alt)
        : m_min(min_alt), m_max(max_alt) {}

protected:
    bool check_condition(const gnss_frame& frame) override
    {
        return frame.fix_valid
            && (frame.altitude_m < m_min || frame.altitude_m > m_max);
    }
    dtc_id get_dtc_id() const override { return dtc::ALTITUDE_RANGE; }

private:
    float m_min;
    float m_max;
};

/**
 * GNSS_CLOCK_JUMP
 *
 * Detects sudden discontinuities in the GNSS-reported timestamp.
 * A healthy GNSS stream should increase monotonically at ~100 ms per frame.
 */
class clock_jump_strategy : public IMonitorStrategy
{
public:
    explicit clock_jump_strategy(float max_jump_ms)
        : m_max_jump_ms(max_jump_ms), m_has_prev(false), m_prev_ts(0)
    {}

protected:
    bool check_condition(const gnss_frame& frame) override
    {
        if (!m_has_prev)
        {
            m_prev_ts  = frame.timestamp;
            m_has_prev = true;
            return false;
        }

        int64_t delta = static_cast<int64_t>(frame.timestamp) - static_cast<int64_t>(m_prev_ts);
        m_prev_ts = frame.timestamp;

        // Both large positive jumps and negative (backward) jumps are faults
        float abs_delta_ms = std::abs(static_cast<float>(delta));
        if (abs_delta_ms > m_max_jump_ms)
            return true;

        return false;
    }

    dtc_id get_dtc_id() const override { return dtc::CLOCK_JUMP; }

private:
    float        m_max_jump_ms;
    bool         m_has_prev;
    timestamp_ms m_prev_ts;
};

/** GNSS_LEAP_SECOND_INVALID */
class leap_second_strategy : public IMonitorStrategy
{
protected:
    bool check_condition(const gnss_frame& frame) override
    {
        return !frame.leap_second_valid;
    }
    dtc_id get_dtc_id() const override { return dtc::LEAP_SECOND_INVALID; }
};

/**
 * no_data_timeout_strategy
 *
 * Detects when no frame has arrived within the configured timeout window.
 * Unlike other strategies, this must be driven by a watchdog tick,
 * not by incoming frames. Call check_timeout() from a separate timer thread
 * or from gnss_data_pipeline::tick_watchdog().
 */
class no_data_timeout_strategy
{
public:
    explicit no_data_timeout_strategy(uint32_t timeout_ms)
        : m_timeout_ms(timeout_ms), m_last_frame_ts(0), m_initialized(false)
    {}

    void on_frame_received(timestamp_ms ts)
    {
        m_last_frame_ts = ts;
        m_initialized   = true;
    }

    /** Call periodically (e.g., every 100 ms) with current wall-clock time */
    bool check_timeout(timestamp_ms now_ms) const
    {
        if (!m_initialized) return false;
        return (now_ms - m_last_frame_ts) > m_timeout_ms;
    }

private:
    uint32_t     m_timeout_ms;
    timestamp_ms m_last_frame_ts;
    bool         m_initialized;
};

// ============================================================
//  SECTION 14: gnss_data_pipeline – Top-Level Facade
// ============================================================

/**
 * gnss_data_pipeline
 *
 * Facade that assembles all components and exposes a simple API:
 *
 *   pipeline.push_frame(frame)   – called on every incoming GNSS frame
 *   pipeline.tick_watchdog(now)  – called from a 100 ms timer
 *
 * Internally:
 *   1. Posts typed events to event_bus
 *   2. Runs all monitor strategies → feeds dtc_manager
 *   3. dtc_manager state changes → reaction_dispatcher chain
 *
 * Construction helper: gnss_data_pipeline::build_default() creates
 * a fully wired instance with default thresholds.
 */
class gnss_data_pipeline
{
public:
    gnss_data_pipeline()
    {
        // Wire dtc_manager → reaction_dispatcher
        m_dtc_manager.set_state_callback(
            [this](const dtc_context& ctx, bool is_set)
            {
                m_dispatcher.dispatch(ctx, is_set);
            }
        );
    }

    // --------------------------------------------------------
    //  Configuration
    // --------------------------------------------------------

    void register_dtc(const dtc_definition& def)
    {
        m_dtc_manager.register_dtc(def);
    }

    void add_monitor(std::unique_ptr<IMonitorStrategy> strategy)
    {
        m_monitors.push_back(std::move(strategy));
    }

    void add_reaction_handler(std::shared_ptr<IReactionHandler> handler)
    {
        m_dispatcher.add_handler(std::move(handler));
    }

    void subscribe_event(event_type type, IEventListener* listener)
    {
        m_bus.subscribe(type, listener);
    }

    void subscribe_all_events(IEventListener* listener)
    {
        m_bus.subscribe_all(listener);
    }

    void set_watchdog_timeout(uint32_t timeout_ms)
    {
        m_no_data_wdg = std::make_unique<no_data_timeout_strategy>(timeout_ms);
    }

    // --------------------------------------------------------
    //  Runtime API
    // --------------------------------------------------------

    /**
     * Called for each incoming GNSS frame.
     * Thread-safety: must be called from a single producer thread.
     */
    void push_frame(const gnss_frame& frame)
    {
        // 1. Update watchdog
        if (m_no_data_wdg)
            m_no_data_wdg->on_frame_received(frame.timestamp);

        // 2. Post semantic events to event_bus before running monitors
        post_frame_events(frame);

        // 3. Run all monitor strategies
        for (auto& monitor : m_monitors)
            monitor->evaluate(frame, m_dtc_manager);

        m_last_frame = frame;
        m_frame_count++;
    }

    /**
     * Call this from a periodic timer (e.g., every 100 ms) with current time.
     * Handles timeout watchdog and any time-driven diagnostics.
     */
    void tick_watchdog(timestamp_ms now_ms)
    {
        if (!m_no_data_wdg) return;

        if (m_no_data_wdg->check_timeout(now_ms))
        {
            // Build a synthetic frame for reporting
            gnss_frame timeout_frame;
            timeout_frame.timestamp = now_ms;
            m_dtc_manager.report_fail(dtc::NO_DATA_TIMEOUT, timeout_frame);

            m_bus.post(IEvent{
                event_type::NO_DATA_TIMEOUT,
                now_ms,
                timeout_frame,
                "No GNSS data received within timeout window"
            });
        }
        else
        {
            gnss_frame ok_frame = m_last_frame;
            ok_frame.timestamp  = now_ms;
            m_dtc_manager.report_pass(dtc::NO_DATA_TIMEOUT, ok_frame);
        }
    }

    // --------------------------------------------------------
    //  Accessors
    // --------------------------------------------------------

    const dtc_manager& get_dtc_manager() const { return m_dtc_manager; }
    dtc_manager&       get_dtc_manager()        { return m_dtc_manager; }

    uint64_t frame_count() const { return m_frame_count; }

    // --------------------------------------------------------
    //  Factory: build a fully wired default pipeline
    // --------------------------------------------------------

    static gnss_data_pipeline build_default(
        const gnss_diag_config& cfg           = gnss_diag_config{},
        std::shared_ptr<IReactionHandler> extra_handler = nullptr)
    {
        gnss_data_pipeline pipeline;

        // --- Register DTC catalogue ---
        const std::vector<dtc_definition> catalogue = {
            { dtc::NO_FIX,              "GNSS_NO_FIX",            "No valid GNSS fix",                           dtc_severity::HIGH,     5,  3  },
            { dtc::SAT_COUNT_LOW,       "GNSS_SAT_COUNT_LOW",     "Fewer than min satellites used in fix",       dtc_severity::MEDIUM,   5,  5  },
            { dtc::SIGNAL_WEAK,         "GNSS_SIGNAL_WEAK",       "Average CNR below threshold",                 dtc_severity::LOW,      8,  5  },
            { dtc::HDOP_HIGH,           "GNSS_HDOP_HIGH",         "Horizontal accuracy degraded (HDOP)",         dtc_severity::MEDIUM,   5,  5  },
            { dtc::PDOP_HIGH,           "GNSS_PDOP_HIGH",         "3D accuracy degraded (PDOP)",                 dtc_severity::MEDIUM,   5,  5  },
            { dtc::POSITION_JUMP,       "GNSS_POSITION_JUMP",     "Implausible position discontinuity",          dtc_severity::HIGH,     3,  5  },
            { dtc::VELOCITY_SPIKE,      "GNSS_VELOCITY_SPIKE",    "Implausible speed detected",                  dtc_severity::HIGH,     3,  5  },
            { dtc::NO_DATA_TIMEOUT,     "GNSS_NO_DATA_TIMEOUT",   "No GNSS frame within timeout window",         dtc_severity::CRITICAL, 1,  3  },
            { dtc::ALTITUDE_RANGE,      "GNSS_ALTITUDE_RANGE",    "Altitude outside plausible range",            dtc_severity::LOW,      5,  5  },
            { dtc::CLOCK_JUMP,          "GNSS_CLOCK_JUMP",        "GNSS timestamp jumped unexpectedly",          dtc_severity::MEDIUM,   3,  5  },
            { dtc::LEAP_SECOND_INVALID, "GNSS_LEAP_SEC_INVALID",  "GNSS leap second flag not valid",             dtc_severity::INFO,    10, 10  },
        };

        for (const auto& def : catalogue)
            pipeline.register_dtc(def);

        // --- Add monitor strategies ---
        pipeline.add_monitor(std::make_unique<no_fix_strategy>());
        pipeline.add_monitor(std::make_unique<sat_count_low_strategy>(cfg.min_used_sats));
        pipeline.add_monitor(std::make_unique<signal_weak_strategy>(cfg.min_cnr_db, cfg.cnr_window_size));
        pipeline.add_monitor(std::make_unique<hdop_high_strategy>(cfg.max_hdop));
        pipeline.add_monitor(std::make_unique<pdop_high_strategy>(cfg.max_pdop));
        pipeline.add_monitor(std::make_unique<position_jump_strategy>(cfg.max_position_jump_m));
        pipeline.add_monitor(std::make_unique<velocity_spike_strategy>(cfg.max_speed_kmh));
        pipeline.add_monitor(std::make_unique<altitude_range_strategy>(cfg.min_altitude_m, cfg.max_altitude_m));
        pipeline.add_monitor(std::make_unique<clock_jump_strategy>(cfg.max_clock_jump_ms));
        pipeline.add_monitor(std::make_unique<leap_second_strategy>());

        // --- Wire watchdog ---
        pipeline.set_watchdog_timeout(cfg.no_data_timeout_ms);

        // --- Build reaction chain ---
        auto log_h      = std::make_shared<log_handler>();
        auto notify_h   = std::make_shared<notify_handler>();
        auto recovery_h = std::make_shared<recovery_handler>();
        auto storage_h  = std::make_shared<dtc_storage_handler>();

        // Default recovery actions (no-ops — caller replaces via set_chip_reset_action etc.)
        recovery_h->set_chip_reset_action([](const dtc_context& ctx)
        {
            printf("[RECOVERY-HOOK] Chip reset requested for: %s\n", ctx.def->name.c_str());
        });
        recovery_h->set_fallback_action([](const dtc_context& ctx)
        {
            printf("[RECOVERY-HOOK] Fallback mode activated for: %s\n", ctx.def->name.c_str());
        });

        pipeline.add_reaction_handler(log_h);
        pipeline.add_reaction_handler(notify_h);
        pipeline.add_reaction_handler(recovery_h);
        pipeline.add_reaction_handler(storage_h);

        if (extra_handler)
            pipeline.add_reaction_handler(extra_handler);

        return pipeline;
    }

private:
    // --------------------------------------------------------
    //  Internal helpers
    // --------------------------------------------------------

    void post_frame_events(const gnss_frame& frame)
    {
        bool was_fix = m_last_frame.fix_valid;

        if (!was_fix && frame.fix_valid)
        {
            m_bus.post({ event_type::FIX_ACQUIRED, frame.timestamp, frame, "Fix acquired" });
        }
        else if (was_fix && !frame.fix_valid)
        {
            m_bus.post({ event_type::FIX_LOST, frame.timestamp, frame, "Fix lost" });
        }

        if (frame.fix_valid && frame.hdop > 2.5f)
        {
            m_bus.post({ event_type::FIX_DEGRADED, frame.timestamp, frame, "Fix degraded (HDOP)" });
        }

        if (frame.fix_valid && frame.cnr_avg_db < 25.f)
        {
            m_bus.post({ event_type::SIGNAL_WEAK, frame.timestamp, frame, "Weak signal" });
        }

        if (frame.fix_valid && frame.used_sats < 4)
        {
            m_bus.post({ event_type::SAT_COUNT_LOW, frame.timestamp, frame, "Low sat count" });
        }

        if (!frame.leap_second_valid)
        {
            m_bus.post({ event_type::LEAP_SECOND_INVALID, frame.timestamp, frame, "Leap second not valid" });
        }
    }

    // --------------------------------------------------------
    //  Member data
    // --------------------------------------------------------

    event_bus                                    m_bus;
    dtc_manager                                  m_dtc_manager;
    reaction_dispatcher                          m_dispatcher;
    std::vector<std::unique_ptr<IMonitorStrategy>> m_monitors;
    std::unique_ptr<no_data_timeout_strategy>    m_no_data_wdg;

    gnss_frame m_last_frame;
    uint64_t   m_frame_count = 0;
};

} // namespace gnss_diag

#endif // GNSS_DIAGNOSTICS_IMPL_H
