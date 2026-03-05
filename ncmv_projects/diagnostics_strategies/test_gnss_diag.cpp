/**
 * MIT License
 *
 * Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 *
 * test_gnss_diag.cpp
 * ============================================================
 *
 * Demonstrates the GNSS Diagnostics system by simulating
 * a realistic GNSS location data stream with injected faults.
 *
 * Scenarios tested:
 *  1. Normal operation (all DTCs stay PASSED)
 *  2. Fix loss (GNSS_NO_FIX → FAILED → HEALED)
 *  3. Weak signal (CNR drop for several seconds)
 *  4. Low satellite count
 *  5. High HDOP / position accuracy degradation
 *  6. Position jump (implausible coordinate discontinuity)
 *  7. Velocity spike (unrealistic speed)
 *  8. No-data timeout (chip silence simulation)
 *  9. Clock jump in GNSS timestamp
 */

#include "gnss_diagnostics_impl.h"

#include <cstdio>
#include <cmath>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <functional>

using namespace gnss_diag;

// ============================================================
//  Helper: monotonic wall-clock in milliseconds
// ============================================================
static timestamp_ms now_ms()
{
    using namespace std::chrono;
    return static_cast<timestamp_ms>(
        duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

// ============================================================
//  Helper: print a section separator
// ============================================================
static void print_section(const std::string& title)
{
    printf("\n╔══════════════════════════════════════════════════════╗\n");
    printf("║  %-52s║\n", title.c_str());
    printf("╚══════════════════════════════════════════════════════╝\n");
}

// ============================================================
//  Example upper-layer observer
//  (simulates an HMI or ADAS component reacting to DTCs)
// ============================================================
class hmi_observer : public IReactionObserver
{
public:
    void on_dtc_set(const dtc_context& ctx) override
    {
        printf("[HMI] ⚠  Warning: %s (sev=%s)\n",
               ctx.def->name.c_str(),
               to_string(ctx.def->severity));
    }

    void on_dtc_cleared(const dtc_context& ctx) override
    {
        printf("[HMI] ✓  Cleared: %s\n", ctx.def->name.c_str());
    }
};

// ============================================================
//  Example event listener (raw GNSS event stream logger)
// ============================================================
class event_logger : public IEventListener
{
public:
    void on_event(const IEvent& event) override
    {
        // Only print high-interest events to avoid console flooding
        switch (event.type)
        {
            case event_type::FIX_ACQUIRED:
            case event_type::FIX_LOST:
            case event_type::NO_DATA_TIMEOUT:
                printf("[EVENT] %s @ %llu ms : %s\n",
                       event_name(event.type),
                       (unsigned long long)event.timestamp,
                       event.detail.c_str());
                break;
            default:
                break;
        }
    }

private:
    static const char* event_name(event_type t)
    {
        switch (t)
        {
            case event_type::FIX_ACQUIRED:        return "FIX_ACQUIRED";
            case event_type::FIX_LOST:            return "FIX_LOST";
            case event_type::NO_DATA_TIMEOUT:     return "NO_DATA_TIMEOUT";
            default:                              return "OTHER";
        }
    }
};

// ============================================================
//  Frame builder helpers
// ============================================================

static gnss_frame make_healthy_frame(timestamp_ms ts, int frame_idx)
{
    gnss_frame f;
    f.timestamp         = ts;
    f.fix_valid         = true;
    f.fix_type          = 1;
    f.satellite_count   = 12;
    f.used_sats         = 8;
    f.hdop              = 0.9f;
    f.vdop              = 1.1f;
    f.pdop              = 1.4f;
    f.latitude          = 48.8566 + frame_idx * 0.000005; // slow northbound motion
    f.longitude         = 2.3522;
    f.altitude_m        = 35.0f;
    f.speed_kmh         = 30.0f;
    f.heading_deg       = 0.0f;
    f.cnr_avg_db        = 38.0f;
    f.age_of_diff_s     = 0.0f;
    f.leap_second_valid = true;
    return f;
}

// ============================================================
//  Scenario runner
// ============================================================

static void run_scenario(
    gnss_data_pipeline&          pipeline,
    const std::string&           name,
    int                          num_frames,
    std::function<gnss_frame(int, timestamp_ms)> frame_factory,
    bool                         print_final_status = true)
{
    print_section(name);

    timestamp_ms ts = now_ms();

    for (int i = 0; i < num_frames; ++i)
    {
        gnss_frame f = frame_factory(i, ts);
        pipeline.push_frame(f);
        ts += 100; // simulate 10 Hz stream
    }

    if (print_final_status)
    {
        printf("\n--- DTC Status after scenario ---\n");
        pipeline.get_dtc_manager().print_status();
    }
}

// ============================================================
//  main
// ============================================================
int main()
{
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║       GNSS Location Service – Diagnostic Demo        ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");

    // ---- Build pipeline with default configuration ----------
    gnss_diag_config cfg;
    cfg.min_used_sats       = 4;
    cfg.min_cnr_db          = 28.0f;
    cfg.max_hdop            = 2.0f;
    cfg.max_pdop            = 3.5f;
    cfg.max_position_jump_m = 40.0;
    cfg.max_speed_kmh       = 200.0f;
    cfg.no_data_timeout_ms  = 500;   // 500 ms for faster demo
    cfg.cnr_window_size     = 5;

    auto pipeline = gnss_data_pipeline::build_default(cfg);

    // ---- Wire upper-layer observers --------------------------
    hmi_observer hmi;
    event_logger evlog;

    // Reach into pipeline internals via build extension point:
    // For demo purposes, we subscribe the event logger directly.
    // In a real system you'd pass observers into build_default().
    pipeline.subscribe_event(event_type::FIX_ACQUIRED,    &evlog);
    pipeline.subscribe_event(event_type::FIX_LOST,        &evlog);
    pipeline.subscribe_event(event_type::NO_DATA_TIMEOUT, &evlog);

    // =========================================================
    //  SCENARIO 1 – Normal operation
    // =========================================================
    run_scenario(pipeline, "SCENARIO 1: Normal Operation (20 frames)", 20,
        [](int i, timestamp_ms ts) {
            return make_healthy_frame(ts, i);
        }
    );

    // =========================================================
    //  SCENARIO 2 – Fix loss (10 frames without fix)
    // =========================================================
    run_scenario(pipeline, "SCENARIO 2: Fix Loss (10 frames no fix)", 10,
        [](int i, timestamp_ms ts) {
            gnss_frame f = make_healthy_frame(ts, i);
            f.fix_valid      = false;
            f.satellite_count = 2;
            f.used_sats      = 0;
            f.hdop           = 99.f;
            f.pdop           = 99.f;
            return f;
        }
    );

    // ---- Recovery from fix loss (5 frames) ------------------
    run_scenario(pipeline, "SCENARIO 2b: Fix Recovery (5 frames)", 5,
        [](int i, timestamp_ms ts) { return make_healthy_frame(ts, i); }
    );

    // =========================================================
    //  SCENARIO 3 – Weak signal (CNR drops to 20 dBHz)
    // =========================================================
    run_scenario(pipeline, "SCENARIO 3: Weak Signal CNR=20 dBHz (15 frames)", 15,
        [](int i, timestamp_ms ts) {
            gnss_frame f = make_healthy_frame(ts, i);
            f.cnr_avg_db = 20.0f;  // below threshold of 28
            return f;
        }
    );

    // ---- CNR recovery ----------------------------------------
    run_scenario(pipeline, "SCENARIO 3b: Signal Recovery (10 frames)", 10,
        [](int i, timestamp_ms ts) { return make_healthy_frame(ts, i); }
    );

    // =========================================================
    //  SCENARIO 4 – Low satellite count
    // =========================================================
    run_scenario(pipeline, "SCENARIO 4: Low Sat Count (used_sats=2, 10 frames)", 10,
        [](int i, timestamp_ms ts) {
            gnss_frame f = make_healthy_frame(ts, i);
            f.satellite_count = 4;
            f.used_sats       = 2; // below min_used_sats=4
            return f;
        }
    );

    run_scenario(pipeline, "SCENARIO 4b: Sat Count Recovery (8 frames)", 8,
        [](int i, timestamp_ms ts) { return make_healthy_frame(ts, i); }
    );

    // =========================================================
    //  SCENARIO 5 – High HDOP / PDOP
    // =========================================================
    run_scenario(pipeline, "SCENARIO 5: High HDOP=4.5 PDOP=5.0 (10 frames)", 10,
        [](int i, timestamp_ms ts) {
            gnss_frame f = make_healthy_frame(ts, i);
            f.hdop = 4.5f;  // above max_hdop=2.0
            f.pdop = 5.0f;  // above max_pdop=3.5
            return f;
        }
    );

    run_scenario(pipeline, "SCENARIO 5b: DOP Recovery (8 frames)", 8,
        [](int i, timestamp_ms ts) { return make_healthy_frame(ts, i); }
    );

    // =========================================================
    //  SCENARIO 6 – Position jump
    // =========================================================
    run_scenario(pipeline, "SCENARIO 6: Position Jump ~500m (5 frames)", 5,
        [](int i, timestamp_ms ts) {
            gnss_frame f = make_healthy_frame(ts, i);
            if (i == 2)
            {
                // Inject a ~500 m jump northward (≈ 0.0045 degrees latitude)
                f.latitude += 0.0045;
            }
            return f;
        }
    );

    run_scenario(pipeline, "SCENARIO 6b: Position Stable Recovery (8 frames)", 8,
        [](int i, timestamp_ms ts) { return make_healthy_frame(ts, 5000 + i); }
    );

    // =========================================================
    //  SCENARIO 7 – Velocity spike
    // =========================================================
    run_scenario(pipeline, "SCENARIO 7: Velocity Spike 350 km/h (5 frames)", 5,
        [](int i, timestamp_ms ts) {
            gnss_frame f = make_healthy_frame(ts, i);
            if (i >= 1 && i <= 3)
                f.speed_kmh = 350.0f; // above max_speed_kmh=200
            return f;
        }
    );

    run_scenario(pipeline, "SCENARIO 7b: Speed Normal Recovery (8 frames)", 8,
        [](int i, timestamp_ms ts) { return make_healthy_frame(ts, i); }
    );

    // =========================================================
    //  SCENARIO 8 – No-data timeout
    //  Simulate chip silence: push a frame, then tick watchdog
    //  for 600 ms without any new frame (> timeout_ms = 500)
    // =========================================================
    print_section("SCENARIO 8: No-Data Timeout (chip silence ~600 ms)");
    {
        gnss_frame seed = make_healthy_frame(now_ms(), 0);
        pipeline.push_frame(seed);

        timestamp_ms watchdog_now = seed.timestamp + 100;
        for (int i = 0; i < 6; ++i)
        {
            watchdog_now += 100;
            pipeline.tick_watchdog(watchdog_now);
        }

        // Resume data after silence
        for (int i = 0; i < 5; ++i)
        {
            gnss_frame f = make_healthy_frame(watchdog_now, i);
            watchdog_now += 100;
            pipeline.push_frame(f);
            pipeline.tick_watchdog(watchdog_now);
        }

        printf("\n--- DTC Status after SCENARIO 8 ---\n");
        pipeline.get_dtc_manager().print_status();
    }

    // =========================================================
    //  SCENARIO 9 – Clock jump in GNSS timestamp
    // =========================================================
    run_scenario(pipeline, "SCENARIO 9: Clock Jump +2000 ms (5 frames)", 5,
        [](int i, timestamp_ms ts) {
            gnss_frame f = make_healthy_frame(ts, i);
            if (i == 2)
                f.timestamp += 2000; // inject a 2-second jump
            return f;
        }
    );

    run_scenario(pipeline, "SCENARIO 9b: Clock Stable Recovery (8 frames)", 8,
        [](int i, timestamp_ms ts) { return make_healthy_frame(ts, i); }
    );

    // =========================================================
    //  FINAL STATUS REPORT
    // =========================================================
    print_section("FINAL DTC STATUS REPORT");
    pipeline.get_dtc_manager().print_status();

    printf("\nTotal frames processed: %llu\n",
           (unsigned long long)pipeline.frame_count());

    return 0;
}
