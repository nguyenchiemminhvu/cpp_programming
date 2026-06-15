/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_GNSS_DIAGNOSTIC_MANAGER_H
#define GNSS_DIAGNOSTIC_GNSS_DIAGNOSTIC_MANAGER_H

#pragma once

#include <memory>
#include <vector>

#include "gnss_event_types.h"
#include "i_clock.h"
#include "i_dtc_monitor.h"
#include "i_gnss_event_source.h"
#include "recovery_manager.h"

namespace gnss_diagnostic
{

/*
 * Facade for the entire GNSS diagnostic subsystem.
 *
 * Wires together:
 *   - the GNSS event source (Observer subject)
 *   - a collection of i_dtc_monitor (Strategy / Template Method)
 *   - the recovery_manager (escalation State Machine)
 *
 * Location Service / Application code only ever talks to this class.
 */
class gnss_diagnostic_manager final : public i_gnss_event_listener
{
public:
    gnss_diagnostic_manager(i_gnss_event_source&             events,
                            const i_clock&                   clk,
                            std::unique_ptr<recovery_manager> recovery);

    ~gnss_diagnostic_manager() override;

    gnss_diagnostic_manager(const gnss_diagnostic_manager&) = delete;
    gnss_diagnostic_manager& operator=(const gnss_diagnostic_manager&) = delete;

    /*
     * Register a monitor before start(). Ownership is transferred.
     * The observer is wired to the recovery_manager automatically.
     */
    void register_monitor(std::unique_ptr<i_dtc_monitor> monitor);

    void start();
    void stop();

    /*
     * Periodic tick from the host runtime (e.g. 100 ms).
     * Drives debounce timers and recovery escalation timers.
     */
    void on_tick(std::chrono::steady_clock::time_point now);

    /* Power mode coupling. */
    void pause();
    void resume();

    // i_gnss_event_listener: fan-out to monitors.
    void on_pvt(const pvt_event& ev) override;
    void on_antenna(const antenna_event& ev) override;

private:
    i_gnss_event_source&                          events_;
    const i_clock&                                clk_;
    std::unique_ptr<recovery_manager>             recovery_;
    std::vector<std::unique_ptr<i_dtc_monitor>>   monitors_;

    bool started_ = false;
    bool paused_  = false;
};

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_GNSS_DIAGNOSTIC_MANAGER_H
