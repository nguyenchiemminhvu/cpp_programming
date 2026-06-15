/*
 * MIT License - Copyright (c) 2026 nguyenchiemminhvu@gmail.com
 */
#ifndef GNSS_DIAGNOSTIC_RECOVERY_MANAGER_H
#define GNSS_DIAGNOSTIC_RECOVERY_MANAGER_H

#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

#include "dtc_config.h"
#include "i_clock.h"
#include "i_dtc_monitor.h"
#include "i_recovery_action.h"

namespace gnss_diagnostic
{

/*
 * Owns the escalation state machine.
 *
 * Behaviour:
 *   - When at least one DTC is set and it persists for level1_timeout,
 *     the level-1 action (chip reset) is executed.
 *   - If a DTC is still set level2_timeout after the level-1 action,
 *     the level-2 action (platform reboot) is executed.
 *   - After the level-2 action runs, the manager enters terminal_no_more
 *     and never recovers again until a full system restart.
 *
 * Pause/resume is invoked by the power mode controller; while paused
 * the timers freeze (re-armed with the current "now" on resume).
 */
class recovery_manager final : public i_dtc_observer
{
public:
    recovery_manager(const i_clock&                                 clk,
                     recovery_config                                cfg,
                     std::vector<std::unique_ptr<i_recovery_action>> actions);

    // i_dtc_observer
    void on_dtc_set(std::uint32_t code) override;
    void on_dtc_cleared(std::uint32_t code) override;

    /* Driven from the manager's tick loop. */
    void evaluate(std::chrono::steady_clock::time_point now);

    void pause();
    void resume(std::chrono::steady_clock::time_point now);

    recovery_level current_level() const { return level_; }

private:
    enum class state
    {
        idle,
        armed,         // waiting for level1_timeout
        level1_done,   // waiting for level2_timeout
        terminal,      // no more recovery attempts
    };

    i_recovery_action* find_action(recovery_level lvl) const;

    const i_clock&                                  clk_;
    recovery_config                                 cfg_;
    std::vector<std::unique_ptr<i_recovery_action>> actions_;

    state                                  state_       = state::idle;
    bool                                   paused_      = false;
    std::unordered_set<std::uint32_t>      active_dtcs_;
    std::chrono::steady_clock::time_point  armed_at_{};
    std::chrono::steady_clock::time_point  level1_at_{};
    recovery_level                         level_       = recovery_level::idle;
    std::uint32_t                          last_trigger_= 0u;
};

} // namespace gnss_diagnostic

#endif // GNSS_DIAGNOSTIC_RECOVERY_MANAGER_H
