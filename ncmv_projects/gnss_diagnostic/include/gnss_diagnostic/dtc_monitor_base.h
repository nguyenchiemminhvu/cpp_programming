/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_DTC_MONITOR_BASE_H
#define GNSS_DIAGNOSTIC_DTC_MONITOR_BASE_H

#pragma once

#include "dtc_config.h"
#include "i_clock.h"
#include "i_dtc_monitor.h"
#include "i_dtc_service.h"

namespace gnss_diagnostic
{

/*
 * Template-method base class implementing the trigger/clear debouncing
 * loop that is common to every monitor.
 *
 * Subclasses only need to override one tiny method:
 *   bool condition_currently_failing(now) const
 *
 * The base class owns:
 *   - DTC lifecycle (set / clear via i_dtc_service)
 *   - debounce timing using injected i_clock
 *   - pause / resume semantics for low-power mode
 *   - notification to the recovery_manager via i_dtc_observer
 */
class dtc_monitor_base : public i_dtc_monitor
{
public:
    dtc_monitor_base(dtc_config        cfg,
                     i_dtc_service&    dtc_svc,
                     const i_clock&    clk);

    std::uint32_t dtc_code() const override;
    bool          is_active() const override;

    void evaluate(std::chrono::steady_clock::time_point now) override;

    void pause() override;
    void resume(std::chrono::steady_clock::time_point now) override;
    void reset() override;

    void set_observer(i_dtc_observer* observer) override;

protected:
    /*
     * Returns true if the underlying physical condition is unhealthy
     * at this instant. The base class wraps this raw boolean signal
     * with debounce timing and DTC bookkeeping.
     */
    virtual bool condition_currently_failing(
        std::chrono::steady_clock::time_point now) const = 0;

    const dtc_config& cfg() const { return cfg_; }
    const i_clock&    clock() const { return clk_; }

private:
    dtc_config       cfg_;
    i_dtc_service&   dtc_svc_;
    const i_clock&   clk_;
    i_dtc_observer*  observer_ = nullptr;

    bool paused_     = false;
    bool dtc_active_ = false;

    bool fail_run_   = false;
    bool ok_run_     = false;
    std::chrono::steady_clock::time_point fail_started_{};
    std::chrono::steady_clock::time_point ok_started_{};
};

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_DTC_MONITOR_BASE_H
